#include "stdio.h"
#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNet.h"


void _Button_irq_handler(void * arg);
void Debug_UART_Init(void);
mico_mutex_t printf_mutex;

void Platform_Init(void)
{
  mico_clib_thread_safe_init();

  Debug_UART_Init();
}

void _Button_irq_handler(void *arg)
{
  Button_irq_handler(arg);
}

void Button_Init(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  Button1_CLK_INIT(Button1_CLK, ENABLE);	
  GPIO_InitStructure.GPIO_Pin = Button1_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			
  GPIO_Init(Button1_PORT, &GPIO_InitStructure);
	
  gpio_irq_enable(Button1_PORT, Button1_IRQ_PIN, IRQ_TRIGGER_FALLING_EDGE, _Button_irq_handler, 0);
  gpio_irq_enable(Button1_PORT, Button1_IRQ_PIN, IRQ_TRIGGER_FALLING_EDGE, _Button_irq_handler, 0);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}	



void Debug_UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

   
  mico_rtos_init_mutex( &printf_mutex );
  DEBUG_GPIO_CLK_INIT(DEBUG_USARTx_RX_GPIO_CLK, ENABLE);
  DEBUG_USARTx_CLK_INIT(DEBUG_USARTx_CLK, ENABLE);
  
  /* Configure USART pin*/
  GPIO_PinAFConfig(DEBUG_USARTx_TX_GPIO_PORT, DEBUG_USARTx_TX_SOURCE, DEBUG_USARTx_TX_AF);
  GPIO_PinAFConfig(DEBUG_USARTx_RX_GPIO_PORT, DEBUG_USARTx_RX_SOURCE, DEBUG_USARTx_RX_AF);
  
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = DEBUG_USARTx_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DEBUG_USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USARTx_RX_PIN;
  GPIO_Init(DEBUG_USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
  
  USART_DeInit(DEBUG_USARTx);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USARTx, &USART_InitStructure);
  
  USART_Cmd(DEBUG_USARTx, ENABLE);
}


/* Retarget the C library printf function to the UART. 
  * All printf output will print from the uart.
  */
int fputc(int ch, FILE *f)
{
  if (ch == '\n')  {
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(DEBUG_USARTx, 0x0D);
  }
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(DEBUG_USARTx, (uint8_t) ch);
  return ch;
}
