#include "stdio.h"

#include "main.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"
#include "platform.h"
#include "http_process.h"
#include "flash_configurations.h"

void web_server_thread(void *arg)
{
  http_init();
  
  while(1) {
    http_tick();
  }
  
}