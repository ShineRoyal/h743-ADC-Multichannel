#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"
#include "main.h"

#define USART_REC_LEN           200     //�����������ֽ��� 200


extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;                //����״̬���
extern u8 USART7_RX_BUF[USART_REC_LEN];
extern u16 USART7_RX_STA;     //����״̬���


#define RXBUFFERSIZE   1 //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer




extern UART_HandleTypeDef UART1_Handler; //UART���
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart8;


void uart_init(u32 bound);
void MX_UART7_Init(void);
void MX_UART8_Init(void);
#endif
