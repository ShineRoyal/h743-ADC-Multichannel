#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

int main(void)
{
    u16 adcx;
    //Cache_Enable();                 //打开L1-Cache
    HAL_Init();                           //初始化HAL库
    Stm32_Clock_Init(200, 2, 2, 2); //设置时钟,400Mhz
    delay_init(400);                        //延时初始化
    uart_init(115200);                  //串口初始化
    printf("hello\r\n");
    MX_ADC1_Init();
    while (1)
    {
        HAL_ADC_Start(&hadc1);                               //开启ADC
        HAL_ADC_PollForConversion(&hadc1, 10);               //轮询转换
        for (int i = 0; i < 8; i++)
        {
            printf("%d\t", (u16)HAL_ADC_GetValue(&hadc1));              //返回最近一次ADC1规则组的转换结果
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