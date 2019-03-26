#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

void Uart2_Init(u32 bound);
void Uart2_SendData(u8 dat);
void Uart2_SendString(char *s);
void Uart2_Method(void);
void Uart1_Init(u32 bound);
void Uart1_SendData(u8 dat);
void Uart1_SendString(char *s);
void Uart1_Method(void);

#endif
