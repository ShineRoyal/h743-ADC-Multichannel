#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
/*
8��ͨ��
ÿ10ms�ɼ�һ���㣬һ������100msҪ�ɼ�10��
��Ϊǰ��˫buffer
*/
#define BUF_SIZE (8*10*2)
u16 buffer[BUF_SIZE];

u8 ss1[10]="hello u7\r\n";	
u8 ss2[10]="hello u8\r\n";

int main(void)
{
    //Cache_Enable();                           //��L1-Cache
    HAL_Init();                             //��ʼ��HAL��
    Stm32_Clock_Init(200, 2, 2, 2);                 //����ʱ��,400Mhz
    delay_init(400);                        //��ʱ��ʼ��
    uart_init(115200);                          //���ڳ�ʼ��
    printf("start\r\n");
    MX_UART7_Init();	//��PC
    MX_UART8_Init();	//��485�豸
		//HAL_UART_Transmit(&huart8,ss2,10,100); ��485�豸����10byte����	
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM3_Init(); 	//���ڶ�ʱ����ADC�ɼ�

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, BUF_SIZE);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    while (1)
    {
				HAL_UART_Transmit(&huart7,ss1,10,100);	
				delay_ms(500);
		}
}

//DMA�봫������жϣ�100*(2n+1)ms����
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA Half transfer complete\r\n");
}

//DMA��������жϣ�100*(2n)ms����
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
