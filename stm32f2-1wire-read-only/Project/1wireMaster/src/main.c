#include "main.h"
#include "ds2482.h"


RCC_ClocksTypeDef RCC_Clocks;

static void I2C_Config(void);
static void SysTickConfig(void);
static void TimeOut_UserCallback(void);
void System_Setup(void);

int main(void)
{
  System_Setup();
  SysTickConfig();
  I2C_Config();

  //xTaskCreate(I2CLink, "Link to 1-wire master", configMINIMAL_STACK_SIZE, NULL, (1 | portPRIVILEGE_BIT), NULL);
  
  xTaskCreate(ToggleLed, "LED1", configMINIMAL_STACK_SIZE, NULL, (3 | portPRIVILEGE_BIT), NULL);
  xTaskCreate(ToggleLed2, "LED2", configMINIMAL_STACK_SIZE, NULL, (2 | portPRIVILEGE_BIT), NULL);
  
  vTaskStartScheduler();
  
  for( ;; );
}  

static void SysTickConfig(void)
{
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void System_Setup(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while (1)
  {   
  }
}

static void I2C_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* GPIO Configuration */
  /*Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);
  
  /*Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
    
  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, I2Cx_SCL_SOURCE, GPIO_AF_I2C1);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, I2Cx_SDA_SOURCE, GPIO_AF_I2C1);

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x02;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;


  I2C_Cmd(I2Cx, ENABLE);
  I2C_DeInit(I2Cx);
  I2C_Init(I2Cx, &I2C_InitStructure);
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {}
}

#endif