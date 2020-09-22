#ifndef __usart_SWO_printf_H
#define __usart_SWO_printf_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "usart.h"
#include <stdio.h>

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */



#ifdef __cplusplus
}
#endif

#endif /*__usart_printf_H*/
