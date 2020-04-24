//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2018-10-31
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2018-10-31
//All rights reserved
//******************************************************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
int main(void)
{
	u8 len;	
	u16 times=0; 
	//Cache_Enable();                 //打开L1-Cache
	HAL_Init();				              //初始化HAL库
	Stm32_Clock_Init(200,2,2,2);    //设置时钟,400Mhz 
	delay_init(400);				        //延时初始化
	uart_init(115200);			      	//串口初始化
	printf("hello\r\n");
	while(1)
	{
    if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000);	//发送接收到的数据
			while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nSTM32H7开发板 串口实验\r\n");
				printf("中景园电子\r\n\r\n\r\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");
		}
		delay_ms(1);
	}
}

