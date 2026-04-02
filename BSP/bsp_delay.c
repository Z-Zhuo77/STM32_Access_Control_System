#include "bsp_delay.h"

/**
 * @brief 初始化延时模块（使用TIM2定时器）
 */
void Delay_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 配置TIM2：72MHz / 72 = 1MHz，即1us一个计数
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // 最大计数65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 启用定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief 延时微秒（精确延时）
 */
void Delay_us(uint32_t us)
{
    TIM_SetCounter(TIM2, 0);
    while(TIM_GetCounter(TIM2) < us);
}

/**
 * @brief 延时毫秒（精确延时）
 */
void Delay_ms(uint32_t ms)
{
    uint32_t i;
    for(i = 0; i < ms; i++)
    {
        Delay_us(1000);
    }
}

