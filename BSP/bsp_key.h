#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"

/* 矩阵按键引脚定义 */
#define KEY_ROW1_PIN     GPIO_Pin_4
#define KEY_ROW2_PIN     GPIO_Pin_5
#define KEY_ROW3_PIN     GPIO_Pin_6
#define KEY_ROW4_PIN     GPIO_Pin_7
#define KEY_COL1_PIN     GPIO_Pin_12
#define KEY_COL2_PIN     GPIO_Pin_13
#define KEY_COL3_PIN     GPIO_Pin_14
#define KEY_COL4_PIN     GPIO_Pin_15

#define KEY_ROW_PORT     GPIOA
#define KEY_COL_PORT     GPIOB

/* 按键值定义 */
#define KEY_NONE         0x00
#define KEY_1            0x01
#define KEY_2            0x02
#define KEY_3            0x03
#define KEY_A            0x04
#define KEY_4            0x05
#define KEY_5            0x06
#define KEY_6            0x07
#define KEY_B            0x08
#define KEY_7            0x09
#define KEY_8            0x0A
#define KEY_9            0x0B
#define KEY_C            0x0C
#define KEY_STAR         0x0D
#define KEY_0            0x0E
#define KEY_POUND        0x0F
#define KEY_D            0x10

/* 函数声明 */
void KEY_Init(void);
uint8_t KEY_Scan(void);
uint8_t KEY_GetChar(uint8_t key_value);

#endif
