#include "bsp_hx711.h"
#include "bsp_delay.h"

// 全局变量
uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;

// 校准参数
// 当测量值偏大时，增加该数值
// 当测量值偏小时，减小该数值
#define GapValue 418.5

/**
  * @brief  HX711初始化
  */
void HX711_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// HX711_SCK
	GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HX711_SCK_PORT, &GPIO_InitStructure);
	
	// HX711_DOUT
	GPIO_InitStructure.GPIO_Pin = HX711_DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(HX711_DOUT_PORT, &GPIO_InitStructure);
	
	HX711_SCK_HIGH();
}

/**
  * @brief  读取HX711数据
  */
uint32_t HX711_Read(void)
{
	uint32_t count = 0;
	uint8_t i;
	
	HX711_SCK_LOW();
	Delay_us(1);
	
	// 等待数据就绪
	while(HX711_DOUT_READ());
	
	// 读取24位数据
	for(i = 0; i < 24; i++)
	{
		HX711_SCK_HIGH();
		count = count << 1;
		Delay_us(1);
		HX711_SCK_LOW();
		if(HX711_DOUT_READ())
			count++;
		Delay_us(1);
	}
	
	// 第25个时钟脉冲（通道A，增益128）
	HX711_SCK_HIGH();
	Delay_us(1);
	HX711_SCK_LOW();
	
	// 转换为有符号数
	if(count & 0x800000)
	{
		count = count - 0x1000000;
	}
	
	return count;
}

/**
  * @brief  获取零点（去皮）
  */
void HX711_GetZero(void)
{
	Weight_Maopi = HX711_Read();
}

/**
  * @brief  获取重量（单位：g）
  */
float HX711_GetWeight(void)
{
	HX711_Buffer = HX711_Read();
	Weight_Shiwu = HX711_Buffer - Weight_Maopi;
	
	if(Weight_Shiwu > 0)
	{
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu / GapValue);
	}
	else
	{
		Weight_Shiwu = 0;
	}
	
	return (float)Weight_Shiwu;
}
