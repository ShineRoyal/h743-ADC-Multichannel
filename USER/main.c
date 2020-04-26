#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

int main(void)
{
    u16 adcx;
    //Cache_Enable();                 //��L1-Cache
    HAL_Init();                           //��ʼ��HAL��
    Stm32_Clock_Init(200, 2, 2, 2); //����ʱ��,400Mhz
    delay_init(400);                        //��ʱ��ʼ��
    uart_init(115200);                  //���ڳ�ʼ��
    printf("hello\r\n");
    MX_ADC1_Init();
    while (1)
    {
        HAL_ADC_Start(&hadc1);                               //����ADC
        HAL_ADC_PollForConversion(&hadc1, 10);               //��ѯת��
        for (int i = 0; i < 8; i++)
        {
            printf("%d\t", (u16)HAL_ADC_GetValue(&hadc1));              //�������һ��ADC1�������ת�����
        }
        printf("\r\n");
        delay_ms(500);
    }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}