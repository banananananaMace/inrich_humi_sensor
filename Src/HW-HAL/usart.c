/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stm32l4xx_hal_uart.h"
#include <string.h>
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


volatile uint16_t USART1_RX_STA=0;       //接收状态标记
uint16_t USART2_RX_STA=0;       //接收状态标记
uint16_t USART3_RX_STA=0;       //接收状态标记

uint8_t aRx1Buffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
uint8_t USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

UART_HandleTypeDef UART1_Handler; //UART句柄
UART_HandleTypeDef UART2_Handler; //UART句柄
UART_HandleTypeDef UART3_Handler; //UART句柄
/**以下重定义fputc的方式适合小端模式**/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar( int ch )
#else
#define PUTCHAR_PROTOTYPE int fputc( int ch, FILE *f )
#endif /* __GNUC__ */


PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit( &UART1_Handler, ( uint8_t * )&ch, 1, 0xFFFF );
    return ch;
}
#ifdef USE_FULL_ASSERT
void assert_failed( uint8_t* file, uint32_t line )
{
}
#endif
/**以上重定义fputc的方式适合小端模式**/

void _Error_Handler(char *file, int line);

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}


void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    if(uartHandle->Instance==USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if(uartHandle->Instance==USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */
UART_HandleTypeDef UART1_Handler; //UART句柄
//*************************************************
//函数: void BSP_UART1_SendStr(uint8_t *Data , uint16_t Len)
//功能: STM32向串口2发送字符串
//参数: Len>0则sData为数组，按照Len的大小发送字节；Len=0则sData为字符串，按照字符串结尾‘\0’计算长度；
//返回: 无
//**************************************************
void BSP_UART1_SendStr( uint8_t *sData, uint16_t Len)
{
    //uint16_t i;
    //__HAL_UART_CLEAR_FLAG(USART1, UART_FLAG_TC);
    if(Len>0)
    {
        HAL_UART_Transmit( &UART1_Handler, sData, Len, 0xFFFF );
    }
    else
    {
        HAL_UART_Transmit( &UART1_Handler,sData,strlen((char*)sData), 0xFFFF );
    }
}


void uart1_init(uint32_t bound)
{
    //UART 初始化设置
    UART1_Handler.Instance=USART1;					    //USART1
    UART1_Handler.Init.BaudRate=bound;				    //波特率
    UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
    UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
    UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
    UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式

    UART1_Handler.Init.OverSampling            = UART_OVERSAMPLING_16;
    UART1_Handler.Init.OneBitSampling          = UART_ONE_BIT_SAMPLE_DISABLE;
    UART1_Handler.AdvancedInit.AdvFeatureInit  = UART_ADVFEATURE_NO_INIT;
    //HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
    if( HAL_UART_Init( &UART1_Handler ) != HAL_OK )
    {
        while( 1 ) {  }
        //Error_Handler( );
    }
    HAL_UART_Receive_IT(&UART1_Handler,(uint8_t *)aRx1Buffer,1);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}


//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_Initure;
    if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
        __HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟

        //TX
        GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;			//PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
        GPIO_Initure.Pull=GPIO_PULLUP;//GPIO_NOPULL;//GPIO_PULLUP;			//上拉
        GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;//GPIO_SPEED_FAST;		//高速
        GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1  USARTx_TX_AF
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PA9
        //RX
//				GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
//        GPIO_Initure.Mode=GPIO_MODE_INPUT;		//复用推挽输出
//        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9
//


#if EN_USART1_RX
        __HAL_UART_ENABLE_IT(&UART1_Handler,UART_IT_RXNE);		//开启接收中断
        //HAL_UART_Receive_IT(&UART1_Handler,(uint8_t *)aRx1Buffer,RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
        HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn,3,2);			//抢占优先级3，子优先级3
#endif
    }

}

extern uint8_t SX1280_Buffer[];
////串口中断的回调函数，接收到0d,0a结束
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)//如果是串口1
    {

        //打开就是无协议接受
//			SX1280_Buffer[USART1_RX_STA]=aRx1Buffer[0];
//			 USART1_RX_STA++;

        if((USART1_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART1_RX_STA&0x4000)//接收到了0x0d
            {
                if(aRx1Buffer[0]!=0x0a)
                    USART1_RX_STA=0;//接收错误,重新开始
                else
                {
                    USART1_RX_STA|=0x8000;	//接收完成了

                }
            }
            else //还没收到0X0D
            {
                if(aRx1Buffer[0]==0x0d)
                    USART1_RX_STA|=0x4000;
                else
                {
                    USART1_RX_BUF[USART1_RX_STA&0X3FFF]=aRx1Buffer[0] ;
                    USART1_RX_STA++;
                    if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收
                }
            }
        }
    }

}

//串口1中断服务程序
void USART1_IRQHandler(void)
{
    uint32_t timeout=0;
    HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数
    timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//等待就绪     == HAL_UART_STATE_BUSY_RX
    {
        timeout++;////超时处理
        if(timeout>HAL_MAX_DELAY) break;
    }
    timeout=0;
    while(HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRx1Buffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后(即接收完RXBUFFERSIZE长度的数据，才会进入回调函数)，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if(timeout>HAL_MAX_DELAY) break;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {

    }
    /* USER CODE END Error_Handler_Debug */
}







