#ifndef _SX1280_APP_H
#define _SX1280_APP_H
//#include "sys.h"
#include "stdio.h"
#include "stdint.h"

//brief Used to display firmware version UART flow
#define FIRMWARE_VERSION    ( ( char* )"Firmware Version: 180928" )
#define MODE_LORA

//brief Defines the output power in dBm，//remark The range of the output power is [-18..+13] dBm/
#define TX_OUTPUT_POWER                             10
//brief Defines the buffer size, i.e. the payload size/
#define REC_BUFFER_SIZE                             35
#define SND_BUFFER_SIZE                             40
//brief Number of tick size steps for tx timeout/
#define TX_TIMEOUT_VALUE                            1000 // 1000*1ms=1s
#define TX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_1000_US   //RADIO_TICK_SIZE_1000_US

//brief Number of tick size steps for rx timeout/
#define RX_TIMEOUT_VALUE                            5000 // 5000*1ms=5s
#define RX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_1000_US   //RADIO_TICK_SIZE_1000_US=1ms
#define RX_TIMEOUT_SINGLEMODE                       0000 // 5000*1ms=5s


//各个SX1280重新设置状态的周期
#define APPTIME_0  360000
#define APPTIME_1  20000
#define APPTIME_2  20000
#define APPTIME_3  20000

typedef enum
{
    APP_LOWPOWER,
    APP_RX,
    APP_RX_TIMEOUT,
    APP_RX_ERROR,
    APP_TX,
    APP_TX_TIMEOUT,
} AppStates_t;

extern uint16_t TxIrqMask;
extern uint8_t RecBufferSize;
extern uint8_t RecBuffer[REC_BUFFER_SIZE];

extern uint8_t SndBufferSize ;
extern uint8_t SndBuffer[SND_BUFFER_SIZE];

extern uint8_t SndBuffer[];
//extern void SX1280_Param_Init(void);

extern uint8_t Process_Appstate_0(void);
//uint8_t Test_SX1280_Send(uint8_t send_length);
void SX1280ProcessTXIrqs( void );


void SX1280_Init(uint32_t RF_FREQUENCY );
void SX1280_Enter_LowPower( void );
void SX1280_Exit_LowPower( void );
uint8_t SX1280_SendData( uint8_t *SendBuf,uint8_t BufferSize);




#endif





