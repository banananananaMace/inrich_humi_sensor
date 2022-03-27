#include "stm32l4xx_hal.h"

typedef uint8_t flash_status_t;
#define FLASH_OK  0
#define FLASH_ERR  1
#define FLASH_CHECK_ERR 2

//2019-10-23
#define  Parameter_Basics 0x0801F000
#define Message_cycle_addr (Parameter_Basics)//1个字节
#define Ctrl_cycle_addr (Message_cycle_addr+4) //4个字节
#define	Time_random_addr (Ctrl_cycle_addr+2)//
#define	Alarm_threshold_addr (Time_random_addr+1)//xdf
#define	Alarm_threshold_DOWN_addr (Alarm_threshold_addr+4)//xdf
#define Frequency_point_addr (Alarm_threshold_DOWN_addr+4)


#define SensorID_Basics 0x0800F000

////ZCR 2019-03-27 
//#define  Parameter_Basics                  0x0801F000
//#define Message_cycle_addr  	(Parameter_Basics)//1个字节
//#define Ctrl_cycle_addr     	(Message_cycle_addr+4) //4个字节
//#define Offset_Delay_addr   	(Ctrl_cycle_addr+2)
//#define Frequency_point_addr	(Offset_Delay_addr+4)
//#define	Alarm_threshold_addr	(Frequency_point_addr+1)//xdf
//#define	Time_random_addr		(Alarm_threshold_addr+2)//xdf

void Read_flash_Parameter(void);
void Write_flash_Parameter(void);
void Read_Sensor_ID(void);



flash_status_t Flash_If_Erase(uint32_t Add);

flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len);
flash_status_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len);
flash_status_t Flash_If_Init(void);
flash_status_t Flash_If_DeInit(void);
void flash_write_NoCheck( uint32_t WriteAddr, uint8_t * pBuffer, uint16_t NumToWrite );   















