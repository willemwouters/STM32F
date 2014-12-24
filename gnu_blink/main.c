

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
	gpio_initstructure.GPIO_Pin  =    0x0000 // used for a holder for -OR-
                                    | GPIO_Pin_0    // ADC-10 =
                                    | GPIO_Pin_1    // ADC-11 =
                                  //| GPIO_Pin_2    // ADC-12 =
                                  //| GPIO_Pin_3    // ADC-13 =
                                  //| GPIO_Pin_4    // ADC-14 =
                                  //| GPIO_Pin_5    // ADC-15 =
                                  //  | GPIO_Pin_6    // PWM-T3.1 =
                                  //  | GPIO_Pin_7    // PWM-T3.2 =
                                  //  | GPIO_Pin_8    // PWM-T3.3 =
                                  //  | GPIO_Pin_9    // PWM-T3.4 =
                                  //| GPIO_Pin_10   // UART4 TxD (Diagnostics)
                                  //| GPIO_Pin_11   // UART4 RxD (Diagnostics)
                                  //| GPIO_Pin_12   // "USART3_CK" [ ?? SPARE ?? ]
                                  //| GPIO_Pin_13   // Tamper/RTC = "TP32"
                                  //| GPIO_Pin_14   // OSC32(IN)
                                  //| GPIO_Pin_15;  // OSC32(OUT)
                                ; // Semi-colon placed here for convenience
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

// Timer code
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void SysTick_Handler(void){
        TimingDelay--;
}

