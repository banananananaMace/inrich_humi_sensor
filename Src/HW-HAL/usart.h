#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "main.h"
#include "stm32l4xx_hal_uart.h"
	 
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

	 
#define EN_USART1_RX     1//使能（1）/禁止（0）串口1接收
#define USART1_REC_LEN  			20  	//定义最大接收字节数 20
#define RXBUFFERSIZE   1 //缓存大小
extern void _Error_Handler(char *, int);
void uart1_init(uint32_t bound);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

void BSP_UART1_SendStr( uint8_t *sData, uint16_t Len);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */



#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
/**
  * @}
  */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/** test 

 BSP_UART1_SendStr(USART1_Testbuf,12);
  */







