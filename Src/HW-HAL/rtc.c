#include "rtc.h"
#include "protocol.h"

uint32_t Message_Cycle_Count_down = 0;
uint32_t WakeUpNumber = 0;
RTC_HandleTypeDef RTCHandle;

void RTC_Config(void)
{
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
        Error_Handler();
    }
}

/*******************************************************
*Function Name 	:Handle_WakeUpSource
*Description  	:�жϷ��͵�֡����
*Input					:
*Output					:
*******************************************************/

void Handle_WakeUpSource(void)
{

    if ( READ_REG(RTC->BKP31R) == 0 ) //�ո��ϵ�����
    {
        Send_Frame_Type = 1;
        HAL_PWR_EnableBkUpAccess();//ʹ�ܺ��������
        HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0);
    }
    else if ( READ_REG(RTC->BKP31R) == 1 ) //�ӵ͹���ģʽ�л���
    {
        WRITE_REG( RTC->BKP31R, 0x0 );//RTC->BKP31R���ֵ������¼�Ƿ����͹���
        HAL_PWR_EnableBkUpAccess();
        Message_Cycle_Count_down = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR1);
        if(Message_Cycle_Count_down == 0)
        {
            WakeUpNumber = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR0);
            WakeUpNumber++;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, WakeUpNumber);						
            if (WakeUpNumber % (Message_cycle / WakeUp_TimeBasis) == 0 )
            {
                Send_Frame_Type = 1;
            }
            if (WakeUpNumber % ((Ctrl_cycle * Message_cycle) / WakeUp_TimeBasis) == 0 )
            {
                Send_Frame_Type = 2;
                HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0);
            }
        }
        else
        {
            Message_Cycle_Count_down--;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, Message_Cycle_Count_down);
        }
    }
}




