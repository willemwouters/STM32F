#define USE_STDPERIPH_DRIVER
#include "stm32f2xx.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_usart.h"
#include "mxchipWNET.h"

#include <stdio.h>


void Delay(uint32_t nTime);
USART_InitTypeDef usart1_init_struct;
    
void assert_failed(uint8_t* file, uint32_t line) {
	while(1);
}


/*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void USART1_Init(void)
{
    /* USART configuration structure for USART1 */
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;
                            
    /* GPIOA PIN9 alternative function Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF;
    gpioa_init_struct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    
    /* GPIOA PIN9 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &gpioa_init_struct);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);  
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart1_init_struct.USART_BaudRate = 9600;   
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;  
    usart1_init_struct.USART_StopBits = USART_StopBits_1;   
    usart1_init_struct.USART_Parity = USART_Parity_No ;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);
    /* Enable RXNE interrupt */
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    //NVIC_EnableIRQ(USART1_IRQn);
}

void USART_Send(const char *str)
{
    while (*str)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
}

//Flash orange LED at about 1hz
int main(void)
{
	long cnt = 0;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef  gpio_initstructure;
	gpio_initstructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1;
	gpio_initstructure.GPIO_Speed= GPIO_Speed_50MHz;
	gpio_initstructure.GPIO_Mode = GPIO_Mode_OUT;
	gpio_initstructure.GPIO_OType = GPIO_OType_PP;
	gpio_initstructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &gpio_initstructure ); 
    USART1_Init();
    
	if (SysTick_Config(SystemCoreClock / 1000))
    	while (1);

     mxchipInit();


    for (;;) {
		cnt++;
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
       	Delay(100);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		Delay(100);
	
		printf("Hello World %ld\r\n", cnt);
    }
}


static __IO uint32_t TimingDelay;
void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void SysTick_Handler(void){
        TimingDelay--;
}

