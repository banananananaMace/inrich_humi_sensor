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


/*--------------------�궨��--------------------*/
#define	start_up_time	17 //(��λms)
#define FLASH_OTP_ADDR ((uint32_t)0x1FFF7000)
/*--------------------32λ--------------------*/
uint64_t sensor_id = 0;
uint64_t default_sensor_id=0;
uint8_t otp_sensor_id_buf[6]= {0};
uint8_t sensor_id_buf[6]= {0xA9,0x2E,0x00,0x00,0x00,0x01}; // {0xA9,0x2E,0x0D,0x00,0x00,0x01};
uint32_t sx1280_receive_time = 0; //�ڵ���ʱ����ط����и�ֵ�ݼ���
volatile uint32_t MainRun_Time = 0;
uint32_t RTC_SleepTime = 0;
extern uint32_t Flash_Sensor_ID;

/*--------------------8λ--------------------*/
uint8_t Rssi = 0;      //�ź�ǿ��
uint8_t Buffer[12] = {0};
uint8_t RspAckbuff[30] = {0x00};
uint8_t WriteBuffer[10], ReadBuffer[10];
uint8_t Send_BURST_Count = 0; //�ظ�����BURST��������
uint8_t Send_BURST_Flag = 0;
uint8_t Burst_Status = 0;
uint8_t Send_Frame_Type = 0;

/*--------------------����--------------------*/
void SHTC3_GetData( void );
uint32_t Handle_RFSendflag(void);
void Enter_LowPower_Mode(void);

/*--------------------����--------------------*/
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
    Read_flash_Parameter();//��ȡ����
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    MX_IWDG_Init();
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    SX1280_Init(Frequency_list[Frequency_point]);
    Battery_Voltage_Measure();
    SHTC3_GetData();//��ʪ��
    Handle_RFSendflag();//�������ݱ�־λsensor_id_buf
    HAL_IWDG_Refresh(&hiwdg);//���Ź�ι��
    Enter_LowPower_Mode();
}

void Enter_LowPower_Mode(void)
{
    WRITE_REG( RTC->BKP31R, 0x1 );//��¼RTC�Ĵ��������Ͻ���ػ�ģʽ
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime = ((RTC_SleepTime - MainRun_Time) * 2.048 - start_up_time);
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

void SHTC3_GetData( void )
{
//	ReadBuffer[0]��ReadBuffer[1]:��ʪ��		ReadBuffer[3]��ReadBuffer[4]:�¶�
    WriteBuffer[0] =  0x44;//Low Power Mode command Read RH First
    WriteBuffer[1] =  0xDE;
    hi2c1_status = HAL_I2C_Master_Transmit(&hi2c1, ADDR_SHTC3_Wirte, WriteBuffer, 2, 0xffff);
    hi2c1_status = HAL_I2C_Master_Receive(&hi2c1, ADDR_SHTC3_Read, ReadBuffer, 6, 0xffff);
    if((ReadBuffer[3] == 0xFF) && (ReadBuffer[4] == 0xFF)) //��ֹ���ϵ�ʱ���������ɴ�������ȡ���ݴ�������͸澯֡,0xFFFF��Ӧ�¶�Ϊ129��
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
        Burst_Status = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR20); //��ѯ�澯֡���͵�״̬
        if(Burst_Status == 1) //֮ǰ״̬�Ǹ澯֡,�����͸澯֡
        {
            Send_BURST_Flag = 0;
        }
        else
        {
            Send_BURST_Flag = 1;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 1); //�豸�澯״̬��־λ
        }
        if(Send_Frame_Type == 1)//�ڷ���Messageʱ��������澯���򲻷���message
        {
            Send_BURST_Flag = 1;
            Send_Frame_Type = 0;
        }
        if(Send_Frame_Type == 2)//�ڷ�REQ��������澯����REQ��BURST
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
        Radio.SetRfFrequency(Frequency_list[1]);//����Ƶ��
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
                    SX1280ProcessIrqs();//ɨ�����״̬�����緢�ͺͽ��գ�����AppStateΪ��Ӧ������Ȼ��ͨ��������봦������
                    IrqState = false;
                }
                Return_Value = Process_Appstate_0();
                if(Return_Value == APP_RX)
                {
                    Return_Value = Handle_receiveStation_sx1280_frame();
                    if(Return_Value == ACK) //���ܵ�Ӧ��֡ȷ��
                    {
                        break; //������ʱѭ��
                    }
                }
            }//����ʱ ��ʱ
            if(Return_Value == ACK) //���ܵ�Ӧ��֡ȷ��
            {
                break; //�����ظ�����ѭ��
            }
        }//Send_BURST_Count �ظ����ʹ���
    }//BURST֡����
    if(Send_Frame_Type == 1) //����message
    {
        Send_Frame_Type = 0;
        SendtoStation_sx1280_frame(MESSAGE, 20, Humiture_type, Buffer);
        HAL_Delay(30);//���͵���ֵ
        Get_random();
    }
    else if(Send_Frame_Type == 2)//����REQ
    {
        Radio.SetRfFrequency(Frequency_list[1]);//�л�����Ƶ��
        Send_Frame_Type = 0;
        AppState = 0;
        sx1280_receive_time = 70; //��ʱ����ܳ�֡
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, Humiture_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState == true)
            {
                SX1280ProcessIrqs();//ɨ�����״̬�����緢�ͺͽ��գ�����AppStateΪ��Ӧ������Ȼ��ͨ��������봦������
                IrqState = false;
            }
            Return_Value = Process_Appstate_0();
            if(Return_Value == APP_RX)
            {
                Return_Value = Handle_receiveStation_sx1280_frame();
                if(Return_Value == RSP_END)
                {
                    Write_flash_Parameter();//�����ܵ������ò���д��flash��
                    Shift_time = Offset_Delay % WakeUp_TimeBasis; //ƫ��ʱ��(С��һ���������ڵ�ʱ��)
                    RTC_SleepTime = (Shift_time + RTC_SleepTime - 23); //����reqʱ��
                    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, ((Offset_Delay % Message_cycle / WakeUp_TimeBasis))); //��ʱһ��ҵ������5����=30��10s����
                    //���óɹ���ظ�һ��ACK
                    Read_flash_Parameter();//��������Ŀ����ȷ������������ȷ��ͨ��ACK֡����;
                    RspAckbuff[0] = 1;
                    SendtoStation_sx1280_frame(ACK, 1, RSP_END_ACK_type, RspAckbuff);
                    HAL_Delay(20);//���͵���ֵ
                    break;//������ʱ
                }
            }
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}
