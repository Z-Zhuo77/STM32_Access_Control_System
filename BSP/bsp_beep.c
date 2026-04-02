#include "bsp_beep.h"

/**
 * @brief 初始化蜂鸣器
 */
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE);
    
    // 配置GPIO为推挽输出
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);
    
    // 默认关闭蜂鸣器（高电平）
    GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/**
 * @brief 打开蜂鸣器（低电平触发）
 */
void BEEP_On(void)
{
    GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/**
 * @brief 关闭蜂鸣器
 */
void BEEP_Off(void)
{
    GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}

/**
 * @brief 切换蜂鸣器状态
 */
void BEEP_Toggle(void)
{
    GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN, (BitAction)(1 - GPIO_ReadOutputDataBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN)));
}

