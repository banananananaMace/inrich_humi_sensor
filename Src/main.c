/*--------------------Includes--------------------*/
#include "string.h"
#include "main.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "protocol.h"
#include "flash.h"
#include "adc.h"
#include "rtc.h"
#include "iwdg.h"
#include "rng.h"


/*--------------------宏定义--------------------*/
#define	start_up_time	17 //(单位ms)
#define FLASH_OTP_ADDR ((uint32_t)0x1FFF7000)
/*--------------------32位--------------------*/
uint64_t sensor_id = 0;
uint64_t default_sensor_id=0;
uint8_t otp_sensor_id_buf[6]= {0};
uint8_t sensor_id_buf[6]= {0xA9,0x2E,0x00,0x00,0x00,0x01}; // {0xA9,0x2E,0x0D,0x00,0x00,0x01};
uint32_t sx1280_receive_time = 0; //在倒计时处理地方进行赋值递减。
volatile uint32_t MainRun_Time = 0;
uint32_t RTC_SleepTime = 0;
extern uint32_t Flash_Sensor_ID;

/*--------------------8位--------------------*/
uint8_t Rssi = 0;      //信号强度
uint8_t Buffer[12] = {0};
uint8_t RspAckbuff[30] = {0x00};
uint8_t WriteBuffer[10], ReadBuffer[10];
uint8_t Send_BURST_Count = 0; //重复发送BURST次数定义
uint8_t Send_BURST_Flag = 0;
uint8_t Burst_Status = 0;
uint8_t Send_Frame_Type = 0;

/*--------------------函数--------------------*/
void SHTC3_GetData( void );
uint32_t Handle_RFSendflag(void);
void Enter_LowPower_Mode(void);

/*--------------------其他--------------------*/
PacketStatus_t packetStatus;
HAL_StatusTypeDef hi2c1_status;
RadioStatus_t s;
float Shift_time = 0.0;
float temperatrue = 0;
uint16_t humiture = 0;
int main( void )
{
    RTC_SleepTime = WakeUp_TimeBasis;
    HwInit();
    Read_flash_Parameter();//读取参数
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    MX_IWDG_Init();
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    SX1280_Init(Frequency_list[Frequency_point]);
    Battery_Voltage_Measure();
    SHTC3_GetData();//温湿度
    Handle_RFSendflag();//发送数据标志位sensor_id_buf
    HAL_IWDG_Refresh(&hiwdg);//看门狗喂狗
    Enter_LowPower_Mode();
}

void Enter_LowPower_Mode(void)
{
    WRITE_REG( RTC->BKP31R, 0x1 );//记录RTC寄存器，马上进入关机模式
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime = ((RTC_SleepTime - MainRun_Time) * 2.048 - start_up_time);
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

void SHTC3_GetData( void )
{
//	ReadBuffer[0]、ReadBuffer[1]:温湿度		ReadBuffer[3]、ReadBuffer[4]:温度
    WriteBuffer[0] =  0x44;//Low Power Mode command Read RH First
    WriteBuffer[1] =  0xDE;
    hi2c1_status = HAL_I2C_Master_Transmit(&hi2c1, ADDR_SHTC3_Wirte, WriteBuffer, 2, 0xffff);
    hi2c1_status = HAL_I2C_Master_Receive(&hi2c1, ADDR_SHTC3_Read, ReadBuffer, 6, 0xffff);
    if((ReadBuffer[3] == 0xFF) && (ReadBuffer[4] == 0xFF)) //防止刚上电时冲击电流造成传感器读取数据错误而发送告警帧,0xFFFF对应温度为129°
    {
        ReadBuffer[3] = 0;
        ReadBuffer[4] = 0;
    }
    temperatrue = (ReadBuffer[3] << 8) + ReadBuffer[4];
    humiture = ((ReadBuffer[0]) << 8) + ReadBuffer[1];
    temperatrue = ((175.0 * temperatrue / 65536) - 45);
    humiture = 100.0 * humiture / 65536;
    if ((temperatrue > Alarm_threshold)||(temperatrue < Alarm_threshold_DOWN))
    {
        Burst_Status = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR20); //查询告警帧发送的状态
        if(Burst_Status == 1) //之前状态是告警帧,不发送告警帧
        {
            Send_BURST_Flag = 0;
        }
        else
        {
            Send_BURST_Flag = 1;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 1); //设备告警状态标志位
        }
        if(Send_Frame_Type == 1)//在发送Message时如果产生告警，则不发送message
        {
            Send_BURST_Flag = 1;
            Send_Frame_Type = 0;
        }
        if(Send_Frame_Type == 2)//在发REQ如果产生告警，发REQ和BURST
        {
            Send_BURST_Flag = 1;
        }
    }
    else
    {
        HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 0);
    }
    memcpy(Buffer,&temperatrue,4);
    memcpy(&Buffer[4],&humiture,2);
}

uint32_t Handle_RFSendflag(void)
{
    uint8_t Return_Value = 0;
    if(Send_BURST_Flag == 1)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//控制频点
        Send_BURST_Flag = 0;
        AppState = 0;
        Send_BURST_Count = 3;
        while(Send_BURST_Count)
        {
            Send_BURST_Count--;
            Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
            SendtoStation_sx1280_frame(BURST, 20, Humiture_type, Buffer);
            sx1280_receive_time = 50;
            while(sx1280_receive_time)
            {
                if( IrqState == true)
                {
                    SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                    IrqState = false;
                }
                Return_Value = Process_Appstate_0();
                if(Return_Value == APP_RX)
                {
                    Return_Value = Handle_receiveStation_sx1280_frame();
                    if(Return_Value == ACK) //接受到应答帧确认
                    {
                        break; //跳出超时循环
                    }
                }
            }//倒计时 超时
            if(Return_Value == ACK) //接受到应答帧确认
            {
                break; //跳出重复次数循环
            }
        }//Send_BURST_Count 重复发送次数
    }//BURST帧处理
    if(Send_Frame_Type == 1) //发送message
    {
        Send_Frame_Type = 0;
        SendtoStation_sx1280_frame(MESSAGE, 20, Humiture_type, Buffer);
        HAL_Delay(30);//发送调整值
        Get_random();
    }
    else if(Send_Frame_Type == 2)//发送REQ
    {
        Radio.SetRfFrequency(Frequency_list[1]);//切换控制频点
        Send_Frame_Type = 0;
        AppState = 0;
        sx1280_receive_time = 70; //有时候接受超帧
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, Humiture_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState == true)
            {
                SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                IrqState = false;
            }
            Return_Value = Process_Appstate_0();
            if(Return_Value == APP_RX)
            {
                Return_Value = Handle_receiveStation_sx1280_frame();
                if(Return_Value == RSP_END)
                {
                    Write_flash_Parameter();//将接受到的配置参数写入flash中
                    Shift_time = Offset_Delay % WakeUp_TimeBasis; //偏移时间(小于一个唤醒周期的时间)
                    RTC_SleepTime = (Shift_time + RTC_SleepTime - 23); //配置req时间
                    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, ((Offset_Delay % Message_cycle / WakeUp_TimeBasis))); //延时一个业务周期5分钟=30个10s周期
                    //配置成功后回复一个ACK
                    Read_flash_Parameter();//读参数的目的是确保数据配置正确，通过ACK帧发送;
                    RspAckbuff[0] = 1;
                    SendtoStation_sx1280_frame(ACK, 1, RSP_END_ACK_type, RspAckbuff);
                    HAL_Delay(20);//发送调整值
                    break;//跳出超时
                }
            }
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}
