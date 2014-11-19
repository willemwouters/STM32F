#include "stdlib.h"
#include "stm32f2xx.h"
#include "mico_rtos.h"
#include "platform.h"
#include "mxchipWNet.h"

#define RX_BUFFER_SIZE 512

#ifndef MIN
#define MIN(x,y)  ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

u8 sem_init = 0;
uint32_t rx_size = 0;
static  mico_semaphore_t tx_complete, rx_complete; 

static  mico_semaphore_t wakeup; 
static mico_thread_t uart_wakeup_thread_handler;
static void uart_wakeup_thread(void *arg);

typedef struct
{
  uint32_t  size;
  uint32_t  head;
  uint32_t  tail;
  uint8_t*  buffer;
} ring_buffer_t;

ring_buffer_t rx_buffer;
uint8_t rx_data[RX_BUFFER_SIZE];

static u8 platform_uart_receive_bytes(void* data, uint32_t size);
void Rx_irq_handler(void *arg);

uint32_t ring_buffer_free_space( ring_buffer_t* ring_buffer )
{
  uint32_t tail_to_end = ring_buffer->size - ring_buffer->tail;
  return ((tail_to_end + ring_buffer->head) % ring_buffer->size);
}

uint32_t ring_buffer_used_space( ring_buffer_t* ring_buffer )
{
  uint32_t head_to_end = ring_buffer->size - ring_buffer->head;
  return ((head_to_end + ring_buffer->tail) % ring_buffer->size);
}

u8 ring_buffer_get_data( ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes )
{
  uint32_t head_to_end = ring_buffer->size - ring_buffer->head;
  
  *data = &(ring_buffer->buffer[ring_buffer->head]);
  
  *contiguous_bytes = MIN(head_to_end, (head_to_end + ring_buffer->tail) % ring_buffer->size);
  return 0;
}

u8 ring_buffer_consume( ring_buffer_t* ring_buffer, uint32_t bytes_consumed )
{
  ring_buffer->head = (ring_buffer->head + bytes_consumed) % ring_buffer->size;
  return 0;
}

uint32_t ring_buffer_write( ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length )
{
  uint32_t tail_to_end = ring_buffer->size - ring_buffer->tail;
  
  /* Calculate the maximum amount we can copy */
  uint32_t amount_to_copy = MIN(data_length, (ring_buffer->tail == ring_buffer->head) ? ring_buffer->size : (tail_to_end + ring_buffer->head) % ring_buffer->size);
  
  /* Copy as much as we can until we fall off the end of the buffer */
  memcpy(&ring_buffer->buffer[ring_buffer->tail], data, MIN(amount_to_copy, tail_to_end));
  
  /* Check if we have more to copy to the front of the buffer */
  if (tail_to_end < amount_to_copy)
  {
    memcpy(ring_buffer->buffer, data + tail_to_end, amount_to_copy - tail_to_end);
  }
  
  /* Update the tail */
  ring_buffer->tail = (ring_buffer->tail + amount_to_copy) % ring_buffer->size;
  
  return amount_to_copy;
}

void UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  
  mico_rtos_init_semaphore(&tx_complete, 1);
  mico_rtos_init_semaphore(&rx_complete, 1);
  sem_init = 1;

  
  mico_mcu_powersave_config(mxDisable);

  rx_buffer.buffer = rx_data;
  rx_buffer.size = RX_BUFFER_SIZE;
  rx_buffer.head = 0;
  rx_buffer.tail = 0;

  GPIO_CLK_INIT(USARTx_RX_GPIO_CLK, ENABLE);
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(DMA_CLK_INIT,ENABLE);

  
  NVIC_InitStructure.NVIC_IRQChannel = UART_TX_DMA_Stream_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel                   = USARTx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = (uint8_t) 0x6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x7;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  
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
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
  
#ifdef MCULowPowerMode
  gpio_irq_enable(USARTx_RX_GPIO_PORT, USARTx_IRQ_PIN, IRQ_TRIGGER_FALLING_EDGE, Rx_irq_handler, 0);
  mico_rtos_init_semaphore(&wakeup, 1);
  mico_rtos_create_thread(&uart_wakeup_thread_handler, MICO_APPLICATION_PRIORITY, "UART_WAKEUP", uart_wakeup_thread, 0x500, NULL );
#endif
  
  USART_DeInit(USARTx);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1 ;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  
  USART_Cmd(USARTx, ENABLE);
  USART_DMACmd(USARTx, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
  
  
  DMA_DeInit( UART_RX_DMA_Stream );
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTx_DR_Base;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_BufferSize = 0;
  
  DMA_Init(UART_RX_DMA_Stream, &DMA_InitStructure);

  platform_uart_receive_bytes( rx_buffer.buffer, rx_buffer.size);
 
  mico_mcu_powersave_config(mxEnable);
}


int dl_uart_close(void)
{
  return 0;
}


void Rx_irq_handler(void *arg)
{
  init_clocks();
    GPIO_CLK_INIT(USARTx_RX_GPIO_CLK, ENABLE);
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(DMA_CLK_INIT,ENABLE);
  
  gpio_irq_disable(USARTx_RX_GPIO_PORT, USARTx_IRQ_PIN);
  mico_mcu_powersave_config(mxDisable);
  mico_rtos_set_semaphore(&wakeup);
}

int UART_Send(u8 *sendBuf, u32 bufLen)
{
  DMA_InitTypeDef  DMA_InitStructure;
  
  mico_mcu_powersave_config(mxDisable);
  
  DMA_DeInit(UART_TX_DMA_Stream);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTx_DR_Base;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
  
  /****************** USART will Transmit Specific Command ******************/ 
  /* Prepare the DMA to transfer the transaction command (2bytes) from the
  memory to the USART */  
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sendBuf;
  DMA_InitStructure.DMA_BufferSize = (uint16_t)bufLen;
  DMA_Init(UART_TX_DMA_Stream, &DMA_InitStructure); 
  
  DMA_ITConfig(UART_TX_DMA_Stream, DMA_IT_TC, ENABLE );
  /* Enable the USART DMA requests */
  
  
  /* Clear the TC bit in the SR register by writing 0 to it */
  USART_ClearFlag(USARTx, USART_FLAG_TC);
  
  /* Enable the DMA TX Stream, USART will start sending the command code (2bytes) */
  DMA_Cmd(UART_TX_DMA_Stream, ENABLE);
  
  if(sem_init)
    mico_rtos_get_semaphore(&tx_complete, 10);
  while( ( USARTx->SR & USART_SR_TC )== 0 );
  
  mico_mcu_powersave_config(mxEnable);

  return 0;
}


int UART_Recv(u8 *recvBuf, u32 bufLen, u32 timeOut)
{
  while (bufLen != 0){
    uint32_t transfer_size = MIN(rx_buffer.size / 2, bufLen);
    
    /* Check if ring buffer already contains the required amount of data. */
    if ( transfer_size > ring_buffer_used_space( &rx_buffer ) ) {
      /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
      rx_size = transfer_size;
      if ( mico_rtos_get_semaphore( &rx_complete, timeOut ) != 0 ){
        rx_size = 0;
        return -1;
      }
      /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
      rx_size = 0;
    }
    
    bufLen -= transfer_size;
    
    // Grab data from the buffer
    do
    {
      uint8_t* available_data;
      uint32_t bytes_available;
      
      ring_buffer_get_data( &rx_buffer, &available_data, &bytes_available );
      bytes_available = MIN( bytes_available, transfer_size );
      memcpy( recvBuf, available_data, bytes_available );
      transfer_size -= bytes_available;
      recvBuf = ( (uint8_t*) recvBuf + bytes_available );
      ring_buffer_consume( &rx_buffer, bytes_available );
    } while ( transfer_size != 0 );
  }
  
  if ( bufLen != 0 ) {
    return -1;
  }
  else{
    return 0;
  }
}


static u8 platform_uart_receive_bytes(void* data, uint32_t size)
{
  uint32_t tmpvar; /* needed to ensure ordering of volatile accesses */
  
  UART_RX_DMA_Stream->CR |= DMA_SxCR_CIRC;
  // Enabled individual byte interrupts so progress can be updated
  USART_ITConfig( USARTx, USART_IT_RXNE, ENABLE );
  
  tmpvar = UART_RX_DMA->LISR;
  UART_RX_DMA->LIFCR      |= tmpvar;
  tmpvar = UART_RX_DMA->HISR;
  UART_RX_DMA->HIFCR      |= tmpvar;
  UART_RX_DMA_Stream->NDTR = size;
  UART_RX_DMA_Stream->M0AR = (uint32_t)data;
  UART_RX_DMA_Stream->CR  |= DMA_SxCR_EN;
  
  return 0;
}


void UART_TX_DMA_IRQHandler(void)
{
  if ( (UART_TX_DMA->HISR & UART_TX_DMA_TCIF) != 0 ){
    UART_TX_DMA->HIFCR |= UART_TX_DMA_TCIF;
    if (sem_init)
      mico_rtos_set_semaphore( &tx_complete);
  }    /* TX DMA error */
  if ( (UART_TX_DMA->HISR & UART_TX_DMA_TCIF) != 0 ){
    /* Clear interrupt */
    UART_TX_DMA->HIFCR |= UART_TX_DMA_TCIF;
  }
}

void USARTx_IRQHandler( void )
{
  // Clear all interrupts. It's safe to do so because only RXNE interrupt is enabled
  USARTx->SR = (uint16_t) (USARTx->SR | 0xffff);
  
  // Update tail
  rx_buffer.tail = rx_buffer.size - UART_RX_DMA_Stream->NDTR;
  
  // Notify thread if sufficient data are available
  if ( ( rx_size > 0 ) && ( ring_buffer_used_space( &rx_buffer ) >= rx_size ) && sem_init )
  {
    mico_rtos_set_semaphore( &rx_complete );
    rx_size = 0;
  }
  #ifdef MCULowPowerMode
    mico_rtos_set_semaphore(&wakeup);
  #endif  
}

static void uart_wakeup_thread(void *arg)
{
  while(1){
    if(mico_rtos_get_semaphore(&wakeup, 1000)!=MXCHIP_SUCCESS){
      gpio_irq_enable(USARTx_RX_GPIO_PORT, USARTx_IRQ_PIN, IRQ_TRIGGER_FALLING_EDGE, Rx_irq_handler, 0);
#ifdef MCULowPowerMode
      mico_mcu_powersave_config(mxEnable);
#endif     
    }
  }
}



