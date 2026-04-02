#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "stm32f10x.h"

// 蜂鸣器引脚定义（PB5 - 低电平触发）
#define BEEP_GPIO_PORT    GPIOB
#define BEEP_GPIO_PIN     GPIO_Pin_5
#define BEEP_GPIO_CLK     RCC_APB2Periph_GPIOB

// 蜂鸣器控制函数
void BEEP_Init(void);      // 初始化蜂鸣器
void BEEP_On(void);        // 打开蜂鸣器
void BEEP_Off(void);       // 关闭蜂鸣器
void BEEP_Toggle(void);    // 切换蜂鸣器状态

#endif

