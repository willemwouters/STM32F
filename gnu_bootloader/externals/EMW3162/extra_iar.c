#include <stdlib.h>
#include <string.h>
 #include "stm32f2xx.h"
void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}
 
void __aeabi_memcpy4(void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}
 
void __aeabi_memclr(void *dest, size_t n)
{
    memset(dest, 0, n);
}
 
void __aeabi_memclr4(void *dest, size_t n)
{
    memset(dest, 0, n);
}
 
void __aeabi_memset(void *dest, char c, size_t n)
{
    memset(dest, c, n);
}




void _sbrk_r() { }

