#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
/*
8个通道
每10ms采集一个点，一个周期100ms要采集10个
分为前后双buffer
*/
#define BUF_SIZE (8*10*2)
u16 buffer[BUF_SIZE];

u8 ss1[10]="hello u7\r\n";	
u8 ss2[10]="hello u8\r\n";

int main(void)
{
    //Cache_Enable();                           //打开L1-Cache
    HAL_Init();                             //初始化HAL库
    Stm32_Clock_Init(200, 2, 2, 2);                 //设置时钟,400Mhz
    delay_init(400);                        //延时初始化
    uart_init(115200);                          //串口初始化
    printf("start\r\n");
    MX_UART7_Init();	//接PC
    MX_UART8_Init();	//接485设备
		//HAL_UART_Transmit(&huart8,ss2,10,100); 往485设备发送10byte数据	
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM3_Init(); 	//用于定时触发ADC采集

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, BUF_SIZE);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    while (1)
    {
				HAL_UART_Transmit(&huart7,ss1,10,100);	
				delay_ms(500);
		}
}

//DMA半传输完成中断，100*(2n+1)ms进入
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA Half transfer complete\r\n");
}

//DMA传输完成中断，100*(2n)ms进入
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA transfer complete\r\n");
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
