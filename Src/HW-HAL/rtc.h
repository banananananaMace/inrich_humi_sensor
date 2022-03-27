#ifndef	__RTC_H
#define __RTC_H
#include "hw.h"

//#define SAMPLE_INTERVAL (20*32768/16)   //(5S*32768/16)//1s=2.048

#define WakeUp_TimeBasis 15000//15000//20000  //(20s)
void RTC_Config(void);
void Handle_WakeUpSource(void);
extern RTC_HandleTypeDef RTCHandle;
extern uint32_t WakeUpNumber;
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xFF  /* 32Khz/128 - 1 */
extern uint32_t Message_cycle;
extern uint16_t Ctrl_cycle;
extern uint64_t	sensor_id;
extern uint8_t Send_Frame_Type;

#endif





