#ifndef __HAL_EABOX_H
#define __HAL_EABOX_H


#include "main.h"
/* Definition for USARTx resources **********************************************/
#define GPIO_CLK_INIT					RCC_AHB1PeriphClockCmd

#define USARTx_CLK          	RCC_APB2Periph_USART1
#define USARTx_CLK_INIT		    RCC_APB1PeriphClockCmd

#define USARTx_RX_PIN         GPIO_Pin_10
#define USARTx_RX_SOURCE      GPIO_PinSource10
#define USARTx_RX_GPIO_PORT   GPIOA
#define USARTx_RX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define USARTx_RX_AF          GPIO_AF_USART1

#define USARTx_TX_PIN         GPIO_Pin_9
#define USARTx_TX_SOURCE      GPIO_PinSource9
#define USARTx_TX_GPIO_PORT   GPIOA
#define USARTx_TX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define USARTx_TX_AF          GPIO_AF_USART1

#define USARTx_CTS_PIN        GPIO_Pin_11
#define USARTx_CTS_SOURCE     GPIO_PinSource11
#define USARTx_CTS_GPIO_PORT  GPIOA
#define USARTx_CTS_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define USARTx_CTS_AF         GPIO_AF_USART1

#define USARTx_RTS_PIN        GPIO_Pin_12
#define USARTx_RTS_SOURCE     GPIO_PinSource12
#define USARTx_RTS_GPIO_PORT  GPIOA
#define USARTx_RTS_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define USARTx_RTS_AF         GPIO_AF_USART1

#define USARTx_IRQn           USART1_IRQn
#define USARTx	              USART1
#define USARTx_IRQHandler     USART1_IRQHandler

#define RELAY_LED_OFF					(GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET))			
#define RELAY_LED_ON					(GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET))		

#define READ_RELAY_ON					(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))		//继电器当前状态为开启;		
#define RELAY_ON							(GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET))			//继电器闭合;
#define RELAY_OFF							(GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET))		//继电器断开;

typedef enum
{
  SYSTEM_LED_ALLWAYS_OFF,
  SYSTEM_LED_ALLWAYS_ON,   //chipid_error
  SYSTEM_LED_TWINKLE_WPS,   //undefined
  SYSTEM_LED_TWINKLE_EASYLINK
} Led_status;


void ButtonInit(void);
void RelayInit(void);
void LEDInit(void);
void EXTI9_5_IRQHandler(void);
void EXTI3_IRQHandler(void);
int check_WPS_function(void);
int check_Restore_function(void);
void switchRelay(void);
void displayRelayLed(void);
void readConfiguration(EAbox_st *configData);
void FlashParaUpdate(EAbox_st *configData);
void restoreConfiguration(void);
void setStatusLED(Led_status status);

#endif /* __HAL_EMW3162_H */
