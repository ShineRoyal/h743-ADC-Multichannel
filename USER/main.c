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
#define DMA_BUF_SIZE (8*10*2)
u16 dma_buffer[DMA_BUF_SIZE];

u8 ss1[10] = "hello u7\r\n";
u8 ss2[10] = "hello u8\r\n";

int main(void)
{
    //Cache_Enable();                           //��L1-Cache
    HAL_Init();                             //��ʼ��HAL��
    Stm32_Clock_Init(200, 2, 2, 2);                 //����ʱ��,400Mhz
    delay_init(400);                        //��ʱ��ʼ��
    uart_init(115200);                          //���ڳ�ʼ��
    printf("start\r\n");


    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM3_Init();     //���ڶ�ʱ����ADC�ɼ�

    MX_UART7_Init();    //��PC
    //HAL_UART_Transmit(&huart7,ss1,10,100); //��PC�豸����10byte����
    MX_UART8_Init();    //��485�豸
    //HAL_UART_Transmit(&huart8,ss2,10,100); //��485�豸����10byte����
    MX_TIM6_Init();       //���UART7ʵ�ֲ��������ݽ���

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_buffer, DMA_BUF_SIZE);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    while (1)
    {


    }
}

//��ʱ������ж�
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //ǿ�Ʊ����ɲ�ֹͣ��ʱ��
    if (htim == (&htim6))
    {
        USART7_RX_STA |= 0x8000;
        HAL_TIM_Base_Stop_IT(&htim6);
        /*
        *����ֱ�Ӿ���������д������ݴ������
        *��ֱ�Ӵ�ӡ��������
        */
        if (USART7_RX_STA & 0x8000)
        {
            printf("%s\r\n", USART7_RX_BUF);    //��ӡ��������
            USART7_RX_STA = 0;                  //���������ɱ�ǣ���������
        }

    }
}


//DMA�봫������жϣ�100*(2n+1)ms���룬����ǰ��buffer������
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA Half transfer complete\r\n");
    /*
     *����ֱ�Ӿ���������������
     *����ÿ��ͨ��10�εľ�ֵ
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

//DMA��������жϣ�100*(2n)ms���룬������buffer������
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    //printf("DMA transfer complete\r\n");
    /*
    *����ֱ�Ӿ���������������
    *����ÿ��ͨ��10�εľ�ֵ
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
