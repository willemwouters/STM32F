
#include "stdio.h"
#include "ctype.h"
#include <stm32f2xx.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
//#include "mxchipWNET.h"

void Delay(uint32_t nTime);

int main(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable peripheral Clocks
    // Enable clocks for GPIO Port C
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
     SystemInit (); 
    // Configure Pins
    // Pin PC9 must be configured as an output
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
    // mxchipInit();
   
    // Configure SysTick Timer
    if (SysTick_Config(SystemCoreClock / 1000))
        while (1);

 
    while (1){
        static int ledval = 0;

        // toggle LED
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, (ledval) ? Bit_SET : Bit_RESET);
        ledval = 1 - ledval;
        //Delay (5000);    // wait 250ms
    }
}

// Timer code
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}


void SysTick_Handler(void){
    if (TimingDelay != 0x00)
        TimingDelay--;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    while(1);
}
#endif
