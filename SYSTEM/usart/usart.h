#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"
#include "main.h"

#define USART_REC_LEN           200     //定义最大接收字节数 200


extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;                //接收状态标记
extern u8 USART7_RX_BUF[USART_REC_LEN];
extern u16 USART7_RX_STA;     //接收状态标记


#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer




extern UART_HandleTypeDef UART1_Handler; //UART句柄
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart8;


void uart_init(u32 bound);
void MX_UART7_Init(void);
void MX_UART8_Init(void);
#endif
