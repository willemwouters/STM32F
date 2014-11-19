#ifndef __HAL_EMW3161_H
#define __HAL_EMW3161_H

#define ENTER_CRITICAL        __disable_irq   //disable_IRQ
#define EXIT_CRITICAL         __enable_irq      //enable_IRQ

/* Definition for USARTx resources **********************************************/
#define UART_RX_BUF_SIZE      2048
#define UART_DMA_MAX_BUF_SIZE 512

#define GPIO_CLK_INIT     RCC_AHB1PeriphClockCmd

#define USARTx_CLK            RCC_APB1Periph_USART2
#define USARTx_CLK_INIT       RCC_APB1PeriphClockCmd

#define USARTx_RX_PIN         GPIO_Pin_3
#define USARTx_IRQ_PIN        3
#define USARTx_RX_SOURCE      GPIO_PinSource3
#define USARTx_RX_GPIO_PORT   GPIOA
#define USARTx_RX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define USARTx_RX_AF          GPIO_AF_USART2

#define USARTx_TX_PIN         GPIO_Pin_2
#define USARTx_TX_SOURCE      GPIO_PinSource2
#define USARTx_TX_GPIO_PORT   GPIOA
#define USARTx_TX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define USARTx_TX_AF          GPIO_AF_USART2

#define USARTx_CTS_PIN        GPIO_Pin_0
#define USARTx_CTS_SOURCE     GPIO_PinSource0
#define USARTx_CTS_GPIO_PORT  GPIOA
#define USARTx_CTS_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define USARTx_CTS_AF         GPIO_AF_USART2

#define USARTx_RTS_PIN        GPIO_Pin_1
#define USARTx_RTS_SOURCE     GPIO_PinSource1
#define USARTx_RTS_GPIO_PORT  GPIOA
#define USARTx_RTS_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define USARTx_RTS_AF         GPIO_AF_USART2

#define USARTx_IRQn           USART2_IRQn
#define USARTx                USART2
#define USARTx_IRQHandler     USART2_IRQHandler

#define USARTx_DR_Base        ((uint32_t)USART2 + 0x04)

#define DMA_CLK_INIT             RCC_AHB1Periph_DMA1
#define UART_RX_DMA              DMA1
#define UART_RX_DMA_Stream       DMA1_Stream5
#define UART_RX_DMA_Stream_IRQn  DMA1_Stream5_IRQn
#define UART_RX_DMA_HTIF         DMA_FLAG_HTIF5
#define UART_RX_DMA_TCIF         DMA_FLAG_TCIF5

#define UART_TX_DMA              DMA1
#define UART_TX_DMA_Stream       DMA1_Stream6
#define UART_TX_DMA_Stream_IRQn  DMA1_Stream6_IRQn
#define UART_TX_DMA_TCIF         DMA_FLAG_TCIF6
#define UART_TX_DMA_IRQHandler   DMA1_Stream6_IRQHandler

/* Definition for Button 1 resources **********************************************/
#define Button1_CLK_INIT      RCC_AHB1PeriphClockCmd

#define Button1_PIN           GPIO_Pin_9
#define Button1_IRQ_PIN       9
#define Button1_PORT          GPIOH
#define Button1_CLK           RCC_AHB1Periph_GPIOH

/* Definition for Debug UART  resources **********************************************/

#define DEBUG_GPIO_CLK_INIT         RCC_AHB1PeriphClockCmd

#define DEBUG_USARTx_CLK            RCC_APB1Periph_USART2
#define DEBUG_USARTx_CLK_INIT       RCC_APB1PeriphClockCmd

#define DEBUG_USARTx_RX_PIN         GPIO_Pin_3
#define DEBUG_USARTx_RX_SOURCE      GPIO_PinSource3
#define DEBUG_USARTx_RX_GPIO_PORT   GPIOA
#define DEBUG_USARTx_RX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define DEBUG_USARTx_RX_AF          GPIO_AF_USART2

#define DEBUG_USARTx_TX_PIN         GPIO_Pin_2
#define DEBUG_USARTx_TX_SOURCE      GPIO_PinSource2
#define DEBUG_USARTx_TX_GPIO_PORT   GPIOA
#define DEBUG_USARTx_TX_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define DEBUG_USARTx_TX_AF          GPIO_AF_USART2

#define DEBUG_USARTx                USART2

#endif /* __HAL_EMW3161_H */
