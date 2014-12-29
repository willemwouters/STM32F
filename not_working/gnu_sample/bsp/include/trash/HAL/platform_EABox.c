#include "stdio.h"
#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNet.h"

#define ENTER_CRITICAL  __disable_irq
#define EXIT_CRITICAL   __enable_irq

#ifdef EABox
#include "flash_if.h"
#define RESET_BUTTON_PRESSED_TIME_DEFAULT	 5000 	//[ms] RESET 按钮用作恢复出厂 功能需要被按下的最小时间(包含防抖时间); 
#define WPS_BUTTON_PRESSED_TIME_WPS			 3000 	//[ms] WPS 按钮用作WPS 功能需要被按下的最小时间(包含防抖时间); 
#define WPS_BUTTON_PRESSED_TIME_RELAY		 30 		//[ms] WPS 按钮用作继电器物理控制的最小按压时间(包含防抖时间); 
#define MAGIC_FLAG		0x454D0380	
#define ledStatusNum 6

extern sysStatus_type SystemStatus;  							// 终端系统的状态;
extern u32 MS_TIMER;


Led_status current_led_status = SYSTEM_LED_ALLWAYS_OFF;

int led_status[][ledStatusNum]={{50000, 1, 50000, 1, 50000, 1},        //SYSTEM_LED_ALLWAYS_OFF
															 {1, 50000, 1, 50000, 1, 50000},       	 //SYSTEM_LED_ALLWAYS_ON  
                               {5000, 5000, 5000, 5000, 5000, 5000},   //SYSTEM_LED_TWINKLE_WPS
                               {1000, 1000, 1000, 1000, 1000, 1000},   //SYSTEM_LED_TWINKLE_EASYLINK
                               {1000, 500, 1000, 500, 1000, 500}};     //undefined
int currentLedDelay = 0; 



u32 WPS_Button_pressed_start_time = 0;		//WPS 按钮被按下时Systick  的即时时间;
u32 RESET_Button_pressed_start_time = 0;	// RESET 按钮被按下时Systick  的即时时间;
volatile int WPSchecking = 0;
volatile int Restorechecking = 0;
int relayFunction = 0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
#endif


void UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
   
  GPIO_CLK_INIT(USARTx_RX_GPIO_CLK, ENABLE);
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);

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
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1 ;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  USART_Cmd(USARTx, ENABLE);
}	


#ifdef EABox
/****************************************************************************************
** 函数名称: RelayInit
** 功能描述: 继电器管脚初始化函数
** 参           数: none
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年07月22日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void RelayInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/****************************************************************************************
** 函数名称: LEDInit
** 功能描述: LED 指示灯管脚初始化函数
** 参           数: none
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年08月13日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void LEDInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	PrescalerValue = (uint16_t) (SystemCoreClock /2/10000) - 1;

	/*	配置PB7 为指示的LED	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*	配置PC7 为指示的LED, 由TIMER8驱动	*/
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
  /* Configure green led as output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
		
	/* Enable the TIM8 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  PrescalerValue = (uint16_t) (SystemCoreClock /2/10000) - 1;

  /* Time base configuration */
	TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    /* TIM enable counter */
  //TIM_Cmd(TIM3, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}

void TIM3_IRQHandler(void)
{
  uint16_t capture = 0;
  if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 );
    capture = TIM_GetCapture2(TIM3);
    TIM_SetCompare2(TIM3, capture + led_status[current_led_status][(currentLedDelay++)%ledStatusNum]);
  } 
}



void setStatusLED(Led_status status)
{
	current_led_status = status;
	switch(status){
		case SYSTEM_LED_ALLWAYS_OFF:
			TIM_Cmd(TIM3, DISABLE);
			TIM_ForcedOC2Config(TIM3, TIM_ForcedAction_InActive);
			break;
		case SYSTEM_LED_ALLWAYS_ON:
			TIM_Cmd(TIM3, DISABLE);
			TIM_ForcedOC2Config(TIM3, TIM_ForcedAction_Active);
			break;
	case SYSTEM_LED_TWINKLE_EASYLINK:
			TIM_OC2Init(TIM3, &TIM_OCInitStructure);
		  TIM_SetCounter(TIM3, 0);
			TIM_Cmd(TIM3, ENABLE);
			break;
	case SYSTEM_LED_TWINKLE_WPS:
			TIM_OC2Init(TIM3, &TIM_OCInitStructure);
			TIM_SetCounter(TIM3, 0);
			TIM_Cmd(TIM3, ENABLE);
			break;
		default:
			TIM_Cmd(TIM3, DISABLE);
			TIM_ForcedOC2Config(TIM3, TIM_ForcedAction_InActive);
			break;
	}
}

/****************************************************************************************
** 函数名称: ButtonInit
** 功能描述: EABox 使用的各按钮的初始化
** 参           数: none
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年07月22日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void ButtonInit(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/*	配置A3  脚为物理开关和WPS  功能开关(长按)	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
       GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*	配置B9  脚为软重启开关和恢复出厂  功能开关(长按)	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
       GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);

	/* Configure EXTI Line3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Configure EXTI Line9 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* 设定外部中断3  的中断优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 设定外部中断5-9  的中断优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void switchRelay(void)
{
	if(READ_RELAY_ON)
	{
		RELAY_OFF;
		RELAY_LED_OFF;
	}
	else
	{
		RELAY_ON;
		RELAY_LED_ON;
	}
}

void displayRelayLed(void)
{
	if(READ_RELAY_ON)
		{
			RELAY_LED_ON;
		}
	else
		{
			RELAY_LED_OFF;
		}
}

/****************************************************************************************
** 函数名称: EXTI9_5_IRQHandler
** 功能描述: 外部中断5--9  的处理函数
** 参           数: none
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年08月09日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
    	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == Bit_RESET){
				//按键被按下, 设立当时Systick 时间为标志;
				Restorechecking = 1;
				RESET_Button_pressed_start_time = MS_TIMER;	
			}				
			else
			{// 按键被抬起
				if (Restorechecking && MS_TIMER - RESET_Button_pressed_start_time >  WPS_BUTTON_PRESSED_TIME_RELAY)//按键按下时间未超过设定的长时按压(长时按压及其功能实现在Systick 中实现)
					NVIC_SystemReset();	
			}
			/* Clear the EXTI line 2 pending bit */
			EXTI_ClearITPendingBit(EXTI_Line9);
		}
}

int check_Restore_function(void)
{	
	ENTER_CRITICAL();
	if(Restorechecking)
	{
		if(MS_TIMER - RESET_Button_pressed_start_time > RESET_BUTTON_PRESSED_TIME_DEFAULT){
			Restorechecking = 0;
			EXIT_CRITICAL();
			return 1;
		}
	}
	EXIT_CRITICAL();
	return 0;

}

/****************************************************************************************
** 函数名称: EXTI3_IRQHandler
** 功能描述: 外部中断3  的处理函数
** 参           数: none
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年07月23日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == Bit_RESET){
			WPSchecking = 1;
			WPS_Button_pressed_start_time = MS_TIMER;			
		}
		else{// 按键被抬起
			if (WPSchecking && MS_TIMER - WPS_Button_pressed_start_time >  WPS_BUTTON_PRESSED_TIME_RELAY)//按键按下时间未超过设定的长时按压(长时按压及其功能实现在Systick 中实现)
					switchRelay();		
			WPS_Button_pressed_start_time = 0;
			WPSchecking = 0;
		}
			/* Clear the EXTI line 3 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

int check_WPS_function(void)
{
	ENTER_CRITICAL();
	if(WPSchecking)
	{
		if(MS_TIMER - WPS_Button_pressed_start_time > WPS_BUTTON_PRESSED_TIME_WPS){
			WPSchecking = 0;
			EXIT_CRITICAL();
			return 1;
		}
	}
	EXIT_CRITICAL();
	return 0;

}

void readConfiguration(EAbox_st *configData)
{
	u32 configInFlash;
	configInFlash = PARA_START_ADDRESS;
	memcpy(configData, (void *)configInFlash, sizeof(eabox_config_t));
	if(configData->conf.magicNumber != MAGIC_FLAG){
		restoreConfiguration();
		NVIC_SystemReset();	
	}
}

/****************************************************************************************
** 函数名称: FlashParaUpdate
** 功能描述: 更新Flash 中保存的参数
** 参           数: 无
** 返   回  值: none
** 作　     者: Sevent
** 日  　   期: 2013年08月27日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void FlashParaUpdate(EAbox_st *configData)
{
	uint32_t paraStartAddress, paraEndAddress;
	
	paraStartAddress = PARA_START_ADDRESS;
	paraEndAddress = PARA_END_ADDRESS;
	
	FLASH_If_Init();
	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
	FLASH_If_Write(&paraStartAddress, (u32 *)configData, sizeof(eabox_config_t));
	FLASH_Lock();
}


void restoreConfiguration(void)
{	
	EAbox_st	EAboxInit;
	uint32_t paraStartAddress, paraEndAddress;
	
	paraStartAddress = PARA_START_ADDRESS;
	paraEndAddress = PARA_END_ADDRESS;
	memset(&EAboxInit, 0x0, sizeof(EAbox_st));
	EAboxInit.conf.magicNumber = MAGIC_FLAG;
	
	FLASH_If_Init();
	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
	FLASH_If_Write(&paraStartAddress, (void *)&EAboxInit, sizeof(eabox_config_t));
	FLASH_Lock();
}


#endif



/* Retarget the C library printf function to the UART. 
  * All printf output will print from the uart.
  */
int fputc(int ch, FILE *f)
{
	if (ch == '\n')  {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, 0x0D);
  }
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(USARTx, (uint8_t) ch);
  return ch;
}
