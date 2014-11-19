#ifndef __HAL_EMW3162_H
#define __HAL_EMW3162_H


#define ENTER_CRITICAL        __disable_irq   //disable_IRQ
#define EXIT_CRITICAL         __enable_irq      //enable_IRQ

/* Definition for USARTx resources **********************************************/
#define UART_RX_BUF_SIZE      2048
#define UART_DMA_MAX_BUF_SIZE 512

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

#define USARTx_DR_Base        ((uint32_t)USART1 + 0x04)

#define DMA_CLK_INIT             RCC_AHB1Periph_DMA2
#define UART_RX_DMA_Stream       DMA2_Stream2
#define UART_RX_DMA_Stream_IRQn  DMA2_Stream2_IRQn
#define UART_RX_DMA_HTIF         DMA_FLAG_HTIF2
#define UART_RX_DMA_TCIF         DMA_FLAG_TCIF2

#define UART_TX_DMA_Stream       DMA2_Stream7
#define UART_TX_DMA_Stream_IRQn  DMA2_Stream7_IRQn
#define UART_TX_DMA_TCIF         DMA_FLAG_TCIF7
/* Definition for Button 1 resources **********************************************/
#define Button1_CLK_INIT			RCC_AHB1PeriphClockCmd

#define Button1_PIN         	GPIO_Pin_3
#define Button1_IRQ_PIN				3
#define Button1_PORT   				GPIOA
#define Button1_CLK    				RCC_AHB1Periph_GPIOA


#endif /* __HAL_EMW3162_H */
