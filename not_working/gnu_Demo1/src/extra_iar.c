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




uint16_t I2C_Read_Flag_Status(I2C_TypeDef* I2Cx, uint8_t I2C_Register) {

}

void __heap_base() { }
void __heap_limit() { }
void __microlib_freelist() { }
void _dvprintf() { }
void dgetc() { }
void _dputs() { }
void _dputc() { }



void ___heap_base() { }

void __dvprintf() { }
void __aeabi_uldivmod() { }

void __2snprintf() { }
void _printf_c() { }

void __aeabi_ui2d() { }
void __stderr() { }

void __stdout() { }
void _sbrk_r() { }

void _printf_d() { }
void __aeabi_uidivmod() { }
void __aeabi_uidiv() { }
void __aeabi_f2uiz() { }
void __aeabi_fdiv() { }
void __aeabi_fmul() { }
void __aeabi_ui2f() { }
void __aeabi_cfrcmple() { }
void __rt_ctype_table() { }


void _printf_pre_padding() { }
void _printf_percent() { }
void _printf_flags() { }
void _printf_x() { }
void __2fprintf() { }
void __2printf() { }
void __2sprintf() { }
void _printf_widthspec() { }
void _printf_sizespec() { }
void _printf_longlong_hex() { }

void _printf_u() { }
void _printf_int_dec() { }
void _printf_s() { }
void _printf_str() { }
void _printf_widthprec() { }


