#ifndef __BSP_SYN6288_H
#define __BSP_SYN6288_H

#include "stm32f10x.h"

// SYN6288音乐类型定义
#define SYN6288_MUSIC_NONE    0 // 无背景音乐
#define SYN6288_MUSIC_1      1 // 音乐1
#define SYN6288_MUSIC_2      2 // 音乐2
#define SYN6288_MUSIC_3      3 // 音乐3
#define SYN6288_MUSIC_4      4 // 音乐4
#define SYN6288_MUSIC_5      5 // 音乐5
#define SYN6288_MUSIC_6      6 // 音乐6
#define SYN6288_MUSIC_7      7 // 音乐7
#define SYN6288_MUSIC_8      8 // 音乐8
#define SYN6288_MUSIC_9      9 // 音乐9
#define SYN6288_MUSIC_10     10 // 音乐10
#define SYN6288_MUSIC_11     11 // 音乐11
#define SYN6288_MUSIC_12     12 // 音乐12
#define SYN6288_MUSIC_13     13 // 音乐13
#define SYN6288_MUSIC_14     14 // 音乐14
#define SYN6288_MUSIC_15     15 // 音乐15

// 函数声明
void SYN6288_Init(void);
void SYN6288_PlayText(uint8_t music, uint8_t *text, uint8_t length);

#endif