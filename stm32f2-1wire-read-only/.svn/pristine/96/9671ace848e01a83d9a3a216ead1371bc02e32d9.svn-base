#include "tasks_misc.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

int i = 0;
int j = 0;

void ToggleLed(void * pvParameters)
{
  portTickType xLastWakeTime;
  const portTickType xFrequency = 333;

  xLastWakeTime = xTaskGetTickCount();

  for( ;; )
  {
    if(i==0)
    {
      GPIOG->BSRRL = GPIO_Pin_8;
      i=1;
    }
    else
    {
      GPIOG->BSRRH = GPIO_Pin_8;
      i=0;
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  } 
}

void ToggleLed2(void * pvParameters)
{
  portTickType xLastWakeTime;
  const portTickType xFrequency = 500;

  /* Initialise the xLastWakeTime variable with the current time.*/
  xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */  
  for( ;; )
  {
    if(j==0)
    {
      GPIOG->BSRRL = GPIO_Pin_6;
      j=1;
    }
    else
    {
      GPIOG->BSRRH = GPIO_Pin_6;
      j=0;
    }

    /* Wait for the next cycle. */
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  } 
}

void I2CLink(void * pvParameters)
{
  portTickType xLastWakeTime;
  const portTickType xFrequency = 500;

  /* Initialise the xLastWakeTime variable with the current time.*/
  xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */  
  for( ;; )
  {
    if(j==0)
    {
      GPIOG->BSRRL = GPIO_Pin_6;
      j=1;
    }
    else
    {
      GPIOG->BSRRH = GPIO_Pin_6;
      j=0;
    }

    /* Wait for the next cycle. */
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  } 
}