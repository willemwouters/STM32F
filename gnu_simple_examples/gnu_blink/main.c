#define USE_STDPERIPH_DRIVER
#include "stm32f2xx.h"
#include "stm32f2xx_gpio.h"
 #include "stm32f2xx_rcc.h"
 
void Delay(uint32_t nTime);

void assert_failed(uint8_t* file, uint32_t line) {
	while(1);
}

//Flash orange LED at about 1hz
int main(void)
{
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef  gpio_initstructure;
	gpio_initstructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1;
	gpio_initstructure.GPIO_Speed= GPIO_Speed_50MHz;
	gpio_initstructure.GPIO_Mode = GPIO_Mode_OUT;
	gpio_initstructure.GPIO_OType = GPIO_OType_PP;
	gpio_initstructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &gpio_initstructure ); 
     
	if (SysTick_Config(SystemCoreClock / 1000))
    	while (1);

    for (;;) {
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
       	Delay(1000);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		Delay(1000);
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

