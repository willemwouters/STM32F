

#define USE_STDPERIPH_DRIVER
#include "stm32f2xx.h"
 

//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}



//Flash orange LED at about 1hz
int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // enable the clock to GPIOD
    //GPIOB->MODER = (1 << 26);             // set pin 13 to be general purpose output
    GPIOB->MODER |= GPIO_MODER_MODER0_0;
    for (;;) {
       ms_delay(500);
       GPIOB->ODR ^= (1 << 0);           // Toggle the pin 
    }
}
