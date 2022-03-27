#ifndef __BOARDS_H__
#define __BOARDS_H__

#define ADDR_SHTC3_Wirte 0xE0
#define ADDR_SHTC3_Read 0xE1
#define ADDR_SHS31_Wirte 0x94
#define ADDR_SHS31_Read 0x95
#define ADDR_TMP116_Wirte 0x90
#define ADDR_TMP116_Read 0x91
#define TMP116_Data_Ready 0x20


#define RADIO_NSS_PIN       GPIO_PIN_4
#define RADIO_NSS_PORT      GPIOA

#define RADIO_MOSI_PIN      GPIO_PIN_7
#define RADIO_MOSI_PORT     GPIOA

#define RADIO_MISO_PIN      GPIO_PIN_6
#define RADIO_MISO_PORT     GPIOA

#define RADIO_SCK_PIN       GPIO_PIN_5
#define RADIO_SCK_PORT      GPIOA

#define RADIO_nRESET_PIN    GPIO_PIN_12
#define RADIO_nRESET_PORT   GPIOA

#define RADIO_BUSY_PIN      GPIO_PIN_11
#define RADIO_BUSY_PORT     GPIOA

#define RADIO_DIO1_PIN      GPIO_PIN_0
#define RADIO_DIO_PORT     GPIOA

#define RADIO_DIO2_PIN      GPIO_PIN_2
#define RADIO_DIO2_PORT     GPIOB

#define RADIO_DIO3_PIN      GPIO_PIN_5
#define RADIO_DIO3_PORT     GPIOB

#define V_I2C_PIN        GPIO_PIN_8
#define V_I2C_PORT       GPIOA

#define V_SHT_PIN        GPIO_PIN_1
#define V_SHT_PORT       GPIOB

#define V_BAT_PIN        GPIO_PIN_1
#define V_BAT_PORT       GPIOA

//#define LED_RX_PIN          GPIO_PIN_15
//#define LED_RX_PORT         GPIOA

#define LED_TX_PIN          GPIO_PIN_3
#define LED_TX_PORT         GPIOB



#endif // __BOARDS_H__
