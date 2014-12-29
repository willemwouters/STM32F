#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "stm32f2xx.h"


#ifdef EMW3162
#include "HAL_EMW3162.h"
#endif


#ifdef EABox
#include "HAL_EABox.h"
#endif

#ifdef EMW3161
#include "HAL_EMW3161.h"
#endif

#ifdef mxchipWNet_HA
#include "mxchipWNet_HA.h"
void hal_uart_init(void);
void hal_uart_tx_tick(void);
// void reset_uart_init(void);
// void reset_uart_deinit(void);
// void uart_recv(void);
// int uart_rx_data_length(void);
// int uart_get_rx_buffer(u8* buf, u32 len);
// void uart_flush_rx_buffer(void);
// void uart_send_is_done(void);
int hal_uart_send_data(u8 *buf, u32 len);
int hal_uart_get_one_packet(u8* buf);

void hal_dma_rx_half_handler(void);
void hal_dma_rx_complete_handler(void);

void readConfiguration(mxchipWNet_HA_st *configData);
void updateConfiguration(mxchipWNet_HA_st *configData);
void restoreConfiguration(void);
#endif

void UART_Init(void);
void Button_Init(void);
__weak void Button_irq_handler(void *arg);

#endif /* __PLATFORM_H */
