#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
    #include "includes.h"                   //os ʹ��
#endif

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#if 1
//#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while ((USART1->ISR & 0X40) == 0); //ѭ������,ֱ���������
    USART1->TDR = (u8)ch;
    return ch;
}
#endif

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��   ������ɱ�־
//bit14��   ���յ�0x0d
//bit13~0�� ���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;     //����״̬���

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 bRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
u8 cRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���

UART_HandleTypeDef UART1_Handler; //UART���

//��ʼ��IO ����1
//bound:������
void uart_init(u32 bound)
{
    //UART ��ʼ������
    UART1_Handler.Instance = USART1;                    //USART1
    UART1_Handler.Init.BaudRate = bound;                //������
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //һ��ֹͣλ
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //����żУ��λ
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //�շ�ģʽ
    HAL_UART_Init(&UART1_Handler);                      //HAL_UART_Init()��ʹ��UART1

    HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStruct;

    if (uartHandle->Instance == USART1) //����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();           //ʹ��GPIOAʱ��
        __HAL_RCC_USART1_CLK_ENABLE();          //ʹ��USART1ʱ��

        GPIO_InitStruct.Pin = GPIO_PIN_9;       //PA9
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;     //�����������
        GPIO_InitStruct.Pull = GPIO_PULLUP;         //����
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1; //����ΪUSART1
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);     //��ʼ��PA9

        GPIO_InitStruct.Pin = GPIO_PIN_10;          //PA10
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);     //��ʼ��PA10

#if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);                //ʹ��USART1�ж�ͨ��
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);        //��ռ���ȼ�3�������ȼ�3
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
    if (huart->Instance == USART1) //����Ǵ���1
    {
        if ((USART_RX_STA & 0x8000) == 0) //����δ���
        {
            if (USART_RX_STA & 0x4000) //���յ���0x0d
            {
                if (aRxBuffer[0] != 0x0a)USART_RX_STA = 0; //���մ���,���¿�ʼ
                else USART_RX_STA |= 0x8000; //���������
            }
            else //��û�յ�0X0D
            {
                if (aRxBuffer[0] == 0x0d)USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0] ;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0; //�������ݴ���,���¿�ʼ����
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

//����1�жϷ������
void USART1_IRQHandler(void)
{
    u32 timeout = 0;
    u32 maxDelay = 0x1FFFF;


    HAL_UART_IRQHandler(&UART1_Handler);    //����HAL���жϴ����ú���

    timeout = 0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++;////��ʱ����
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
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

    HAL_UART_IRQHandler(&huart7);    //����HAL���жϴ����ú���

    timeout = 0;
    while (HAL_UART_GetState(&huart7) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++;////��ʱ����
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&huart7, (u8 *)bRxBuffer, RXBUFFERSIZE) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
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

    HAL_UART_IRQHandler(&huart8);    //����HAL���жϴ����ú���

    timeout = 0;
    while (HAL_UART_GetState(&huart8) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++;////��ʱ����
        if (timeout > maxDelay) break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&huart8, (u8 *)cRxBuffer, RXBUFFERSIZE) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if (timeout > maxDelay) break;
    }
    /* USER CODE END UART8_IRQn 1 */
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




