#include "stm32f2xx.h"
#include "mico_rtos.h"

u32  app_stack_size = 6*1024; 

static mico_mutex_t systemMutex = NULL;
static mico_mutex_t fileMutex = NULL;

void mico_clib_thread_safe_init(void)
{
  mico_rtos_init_mutex( &systemMutex );
  mico_rtos_init_mutex( &fileMutex );
}

void __iar_system_Mtxinit(__iar_Rmtx *mutex)    // Initialize a system lock
{
}
void __iar_system_Mtxdst(__iar_Rmtx *mutex)     // Destroy a system lock
{

}
void __iar_system_Mtxlock(__iar_Rmtx *mutex)    // Lock a system lock
{
  if(systemMutex != NULL)
    mico_rtos_lock_mutex( &systemMutex );
}
void __iar_system_Mtxunlock(__iar_Rmtx *mutex)  // Unlock a system lock
{
  if(systemMutex != NULL)
    mico_rtos_unlock_mutex( &systemMutex );
}

void __iar_file_Mtxinit(__iar_Rmtx *mutex)    // Initialize a file lock
{
}
void __iar_file_Mtxdst(__iar_Rmtx *mutex)     // Destroy a file lock
{
}
void __iar_file_Mtxlock(__iar_Rmtx *mutex)    // Lock a file lock
{
  if(fileMutex != NULL)
    mico_rtos_lock_mutex( &fileMutex );
}
void __iar_file_Mtxunlock(__iar_Rmtx *mutex)  // Unlock a file lock
{
  if(fileMutex != NULL)
    mico_rtos_unlock_mutex( &fileMutex );
}
