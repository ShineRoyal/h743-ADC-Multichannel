#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"

u16 buffer[128];
int main(void)
{
    u16 adcx;
    //Cache_Enable();                 //��L1-Cache
    HAL_Init();                           //��ʼ��HAL��
    Stm32_Clock_Init(200, 2, 2, 2); //����ʱ��,400Mhz
    delay_init(400);                        //��ʱ��ʼ��
    uart_init(115200);                  //���ڳ�ʼ��
    printf("hello\r\n");
    MX_DMA_Init();
    MX_ADC1_Init();
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, 16);
    while (1)
    {
        printf("\r\nvalue:");
        for (int i = 0; i < 32; i++)
        {
            if (i % 8 == 0)   printf("\r\n");
            printf("%d\t", buffer[i]);              //�������һ��ADC1�������ת�����
        }
        delay_ms(500);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    printf("DMA transfer complete\r\n");
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    printf("DMA Half transfer complete\r\n");
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    printf("DMA transfer error\r\n");
}



void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}
