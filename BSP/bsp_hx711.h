#ifndef __BSP_HX711_H
#define __BSP_HX711_H

#include "stm32f10x.h"

// HX711引脚定义
#define HX711_SCK_PORT    GPIOB
#define HX711_SCK_PIN     GPIO_Pin_0
#define HX711_DOUT_PORT   GPIOB
#define HX711_DOUT_PIN    GPIO_Pin_1

// 便捷宏定义
#define HX711_SCK_HIGH()  GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN)
#define HX711_SCK_LOW()   GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN)
#define HX711_DOUT_READ() GPIO_ReadInputDataBit(HX711_DOUT_PORT, HX711_DOUT_PIN)

// 函数声明
void HX711_Init(void);
uint32_t HX711_Read(void);
void HX711_GetZero(void);
float HX711_GetWeight(void);

// 全局变量声明
extern uint32_t HX711_Buffer;
extern uint32_t Weight_Maopi;
extern int32_t Weight_Shiwu;

#endif

