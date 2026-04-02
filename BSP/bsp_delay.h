#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "stm32f10x.h"

// 延时函数声明
void Delay_Init(void);      // 初始化延时模块
void Delay_us(uint32_t us); // 延时微秒
void Delay_ms(uint32_t ms); // 延时毫秒

#endif

