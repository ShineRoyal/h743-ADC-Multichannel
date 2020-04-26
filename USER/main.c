#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"

u16 buffer[128];
int main(void)
{
    u16 adcx;
    //Cache_Enable();                 //打开L1-Cache
    HAL_Init();                           //初始化HAL库
    Stm32_Clock_Init(200, 2, 2, 2); //设置时钟,400Mhz
    delay_init(400);                        //延时初始化
    uart_init(115200);                  //串口初始化
    printf("hello\r\n");
    MX_DMA_Init();
    MX_ADC1_Init();
	  MX_TIM3_Init();
	
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, 16);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	

    while (1)
    {
        printf("\r\nvalue:");
        for (int i = 0; i < 32; i++)
        {
            if (i % 8 == 0)   printf("\r\n");
            printf("%d\t", buffer[i]);              //返回最近一次ADC1规则组的转换结果
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
