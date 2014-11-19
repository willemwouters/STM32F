#include "stdio.h"
#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"

#define ENTER_CRITICAL  __disable_irq
#define EXIT_CRITICAL   __enable_irq

void _Button_irq_handler(void * arg);

void UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
   
  GPIO_CLK_INIT(USARTx_RX_GPIO_CLK, ENABLE);
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);

  /* Configure USART pin*/
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1 ;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  USART_Cmd(USARTx, ENABLE);
}	


void Button_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	Button1_CLK_INIT(Button1_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = Button1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			
	GPIO_Init(Button1_PORT, &GPIO_InitStructure);
	
	gpio_irq_enable(Button1_PORT, Button1_IRQ_PIN, IRQ_TRIGGER_FALLING_EDGE, _Button_irq_handler, 0);
}	

void _Button_irq_handler(void *arg)
{
	Button_irq_handler(arg);
}

/* Retarget the C library printf function to the UART. 
  * All printf output will print from the uart.
  */
int fputc(int ch, FILE *f)
{
	if (ch == '\n')  {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, 0x0D);
  }
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(USARTx, (uint8_t) ch);
  return ch;
}
