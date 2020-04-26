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
#define DMA_BUF_SIZE (8*10*2)
u16 dma_buffer[DMA_BUF_SIZE];

u8 ss1[10] = "hello u7\r\n";
u8 ss2[10] = "hello u8\r\n";

int main(void)
{
    //Cache_Enable();                           //打开L1-Cache
    HAL_Init();                             //初始化HAL库
    Stm32_Clock_Init(200, 2, 2, 2);                 //设置时钟,400Mhz
    delay_init(400);                        //延时初始化
    uart_init(115200);                          //串口初始化
    printf("start\r\n");


    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM3_Init();     //用于定时触发ADC采集

    MX_UART7_Init();    //接PC
    //HAL_UART_Transmit(&huart7,ss1,10,100); //往PC设备发送10byte数据
    MX_UART8_Init();    //接485设备
    //HAL_UART_Transmit(&huart8,ss2,10,100); //往485设备发送10byte数据
    MX_TIM6_Init();       //配合UART7实现不定长数据接收

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_buffer, DMA_BUF_SIZE);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    while (1)
    {


    }
}

//定时器溢出中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //强制标记完成并停止定时器
    if (htim == (&htim6))
    {
        USART7_RX_STA |= 0x8000;
        HAL_TIM_Base_Stop_IT(&htim6);
        /*
        *可以直接就在这里进行串口数据处理解析
        *如直接打印串口数据
        */
        if (USART7_RX_STA & 0x8000)
        {
            printf("%s\r\n", USART7_RX_BUF);    //打印串口数据
            USART7_RX_STA = 0;                  //清除接收完成标记，长度清零
        }

    }
}


//DMA半传输完成中断，100*(2n+1)ms进入，处理前半buffer的数据
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA Half transfer complete\r\n");
    /*
     *可以直接就在这里数据运算
     *如求每个通道10次的均值
     */
    int i;
    u32 ch[8];
    for (i = 0; i < 8; i++)    ch[i] = 0;
    for (i = 0; i < 80; i++)
    {
        ch[i % 8] += dma_buffer[i];
    }
    for (i = 0; i < 8; i++) printf("%d\t", ch[i]/10);
    printf("\r\n");
}

//DMA传输完成中断，100*(2n)ms进入，处理后半buffer的数据
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA transfer complete\r\n");
    /*
    *可以直接就在这里数据运算
    *如求每个通道10次的均值
    */
    int i;
    u32 ch[8];
    for (i = 0; i < 8; i++)    ch[i] = 0;
    for (i = 80; i < 160; i++)
    {
        ch[i % 8] += dma_buffer[i];
    }
    for (i = 0; i < 8; i++) printf("%d\t", ch[i]/10);
    printf("\r\n");

}
