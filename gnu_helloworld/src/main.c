
#include "stdio.h"
#include "ctype.h"
#include <stm32f2xx.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
#include "misc.h"

int main(void);
volatile uint32_t sys_ticks; 
void SysTick_Handler(void)__attribute__((weak));
 

void Delay(uint32_t nTime);


void _exit(int i) {
    while(1);
}

__asm__(".word 0x20001000");
__asm__(".word main");

int main(){
   SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK);

   if (SysTick_Config(SystemCoreClock / 1000))
   while (1);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable peripheral Clocks
    // Enable clocks for GPIO Port C
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    // Configure Pins
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
   
    while (1);

    while (1){
        static int ledval = 0;

        // toggle LED
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, (ledval) ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, (ledval) ? Bit_SET : Bit_RESET);

        ledval = 1 - ledval;
        Delay (5000);    // wait 250ms
    }

   return 0;
}

// Timer code
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0) {
	TimingDelay--;
   }
}

void SysTick_Handler(void){
        TimingDelay--;
}

void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    //while(1);
}

