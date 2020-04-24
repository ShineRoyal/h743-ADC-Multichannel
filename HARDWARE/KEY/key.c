#include "key.h"


//按键初始化函数
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟

	GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2;            //PA1,PA2 
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLUP;        //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//按键处理函数
//返回按键值
//注意此函数有响应优先级,KEY0>KEY1>WK_UP
u8 KEY_Scan(void)
{
	if((KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);
		if(KEY0==0)        return 1;
		else if(KEY1==0)   return 2;
		else if(WK_UP==1)  return 3;
	}
	return 0;   //无按键按下
}















