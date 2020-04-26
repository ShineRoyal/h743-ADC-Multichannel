#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
    #include "includes.h"                   //os 使用
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#if 1
//#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART1->ISR & 0X40) == 0); //循环发送,直到发送完毕
    USART1->TDR = (u8)ch;
    return ch;
}
#endif

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，   接收完成标志
//bit14，   接收到0x0d
//bit13~0， 接收到的有效字节数目
u16 USART_RX_STA = 0;     //接收状态标记

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 bRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 cRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART1_Handler; //UART句柄

//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
    //UART 初始化设置
    UART1_Handler.Instance = USART1;                    //USART1
    UART1_Handler.Init.BaudRate = bound;                //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART1_Handler);                      //HAL_UART_Init()会使能UART1

    HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量

}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStruct;

    if (uartHandle->Instance == USART1) //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();           //使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();          //使能USART1时钟

        GPIO_InitStruct.Pin = GPIO_PIN_9;       //PA9
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;     //复用推挽输出
        GPIO_InitStruct.Pull = GPIO_PULLUP;         //上拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1; //复用为USART1
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);     //初始化PA9

        GPIO_InitStruct.Pin = GPIO_PIN_10;          //PA10
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);     //初始化PA10

#if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);                //使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);        //抢占优先级3，子优先级3
#endif
    }
    else if (uartHandle->Instance == UART7)
    {
        /* USER CODE BEGIN UART7_MspInit 0 */

        /* USER CODE END UART7_MspInit 0 */
        /* UART7 clock enable */
        __HAL_RCC_UART7_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**UART7 GPIO Configuration
        PE7     ------> UART7_RX
        PE8     ------> UART7_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_UART7;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* UART7 interrupt Init */
        HAL_NVIC_SetPriority(UART7_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(UART7_IRQn);
        /* USER CODE BEGIN UART7_MspInit 1 */

        /* USER CODE END UART7_MspInit 1 */
    }
    else if (uartHandle->Instance == UART8)
    {
        /* USER CODE BEGIN UART8_MspInit 0 */

        /* USER CODE END UART8_MspInit 0 */
        /* UART8 clock enable */
        __HAL_RCC_UART8_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**UART8 GPIO Configuration
        PE0     ------> UART8_RX
        PE1     ------> UART8_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* UART8 interrupt Init */
        HAL_NVIC_SetPriority(UART8_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(UART8_IRQn);
        /* USER CODE BEGIN UART8_MspInit 1 */

        /* USER CODE END UART8_MspInit 1 */
    }




}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) //如果是串口1
    {
        if ((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if (aRxBuffer[0] != 0x0a)USART_RX_STA = 0; //接收错误,重新开始
                else USART_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (aRxBuffer[0] == 0x0d)USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0] ;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }

    }
    else if (huart->Instance == UART7)
    {
				printf("%c",bRxBuffer[0]);
    }
    else if (huart->Instance == UART8)
    {
					printf("%c",cRxBuffer[0]);
    }

}

//串口1中断服务程序
void USART1_IRQHandler(void)
{
    u32 timeout = 0;
    u32 maxDelay = 0x1FFFF;


    HAL_UART_IRQHandler(&UART1_Handler);    //调用HAL库中断处理公用函数

    timeout = 0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++;////超时处理
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > maxDelay) break;
    }
}
#endif

/**
 ******************************************************************************
 * File Name          : USART.c
 * Description        : This file provides code for the configuration
 *                      of the USART instances.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart8;


/* UART7 init function */
void MX_UART7_Init(void)
{

    huart7.Instance = UART7;
    huart7.Init.BaudRate = 115200;
    huart7.Init.WordLength = UART_WORDLENGTH_8B;
    huart7.Init.StopBits = UART_STOPBITS_1;
    huart7.Init.Parity = UART_PARITY_NONE;
    huart7.Init.Mode = UART_MODE_TX_RX;
    huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart7.Init.OverSampling = UART_OVERSAMPLING_16;
    huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart7) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_Receive_IT(&huart7, (u8 *)bRxBuffer, RXBUFFERSIZE);

}
/* UART8 init function */
void MX_UART8_Init(void)
{

    huart8.Instance = UART8;
    huart8.Init.BaudRate = 115200;
    huart8.Init.WordLength = UART_WORDLENGTH_8B;
    huart8.Init.StopBits = UART_STOPBITS_1;
    huart8.Init.Parity = UART_PARITY_NONE;
    huart8.Init.Mode = UART_MODE_TX_RX;
    huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart8.Init.OverSampling = UART_OVERSAMPLING_16;
    huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart8) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_Receive_IT(&huart8, (u8 *)cRxBuffer, RXBUFFERSIZE);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
void UART7_IRQHandler(void)
{
    /* USER CODE BEGIN UART7_IRQn 0 */

    /* USER CODE END UART7_IRQn 0 */
    HAL_UART_IRQHandler(&huart7);
    /* USER CODE BEGIN UART7_IRQn 1 */
    u32 timeout = 0;
    u32 maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&huart7);    //调用HAL库中断处理公用函数

    timeout = 0;
    while (HAL_UART_GetState(&huart7) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++;////超时处理
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&huart7, (u8 *)bRxBuffer, RXBUFFERSIZE) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > maxDelay) break;
    }
    /* USER CODE END UART7_IRQn 1 */
}

/**
  * @brief This function handles UART8 global interrupt.
  */
void UART8_IRQHandler(void)
{
    /* USER CODE BEGIN UART8_IRQn 0 */

    /* USER CODE END UART8_IRQn 0 */
    HAL_UART_IRQHandler(&huart8);
    /* USER CODE BEGIN UART8_IRQn 1 */
    u32 timeout = 0;
    u32 maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&huart8);    //调用HAL库中断处理公用函数

    timeout = 0;
    while (HAL_UART_GetState(&huart8) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++;////超时处理
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&huart8, (u8 *)cRxBuffer, RXBUFFERSIZE) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > maxDelay) break;
    }
    /* USER CODE END UART8_IRQn 1 */
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




