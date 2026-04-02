#ifndef __BSP_AS608_H
#define __BSP_AS608_H

#include "stm32f10x.h"

// 缓冲区定义
#define CharBuffer1 0x01
#define CharBuffer2 0x02
#define USART1_RX_BUF_SIZE 100

// 外部变量
extern uint32_t AS608Addr;
extern uint8_t USART1_RX_BUF[];
extern uint16_t USART1_RX_STA;

// 搜索结果结构体
typedef struct  
{
    u16 pageID;      // 指定ID
    u16 mathscore;   // 匹配分数
} SearchResult;

// 系统参数结构体
typedef struct
{
    u16 PS_max;      // 指纹最大容量
    u8  PS_level;    // 安全等级
    u32 PS_addr;     // 地址
    u8  PS_size;     // 通信数据包大小
    u8  PS_N;        // 波特率基数N
} SysPara;

/**
  * @brief  AS608初始化
  */
void AS608_Init(void);

/**
  * @brief  验证密码
  */
uint8_t AS608_VerifyPassword(void);

/**
  * @brief  获取指纹图像
  */
uint8_t AS608_GetImage(void);

/**
  * @brief  将图像存入缓冲区
  * @param  buffer_id: 缓冲区ID（1或2）
  */
uint8_t AS608_GenChar(uint8_t buffer_id);

/**
  * @brief  比对两个缓冲区的指纹
  */
uint8_t AS608_Match(void);

/**
  * @brief  搜索指纹
  */
uint8_t AS608_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p);

/**
  * @brief  合并指纹模板
  */
uint8_t AS608_RegModel(void);

/**
  * @brief  存储模板
  */
uint8_t AS608_StoreChar(u8 BufferID, u16 PageID);

/**
  * @brief  删除模板
  */
uint8_t AS608_DeletChar(u16 PageID, u16 N);

/**
  * @brief  清空指纹库
  */
uint8_t AS608_Empty(void);

/**
  * @brief  读取系统参数
  */
uint8_t AS608_ReadSysPara(SysPara *p);

/**
  * @brief  获取有效模板数量
  */
uint8_t AS608_ValidTempleteNum(u16 *ValidN);

/**
  * @brief  写入寄存器
  */
uint8_t AS608_WriteReg(u8 RegNum, u8 DATA);

/**
  * @brief  设置模块地址
  */
uint8_t AS608_SetAddr(u32 PS_addr);

/**
  * @brief  写入记事本
  */
uint8_t AS608_WriteNotepad(u8 NotePageNum, u8 *Byte32);

/**
  * @brief  读取记事本
  */
uint8_t AS608_ReadNotepad(u8 NotePageNum, u8 *Byte32);

/**
  * @brief  高速搜索
  */
uint8_t AS608_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p);

/**
  * @brief  与AS608握手
  */
uint8_t AS608_HandShake(u32 *PS_Addr);

/**
  * @brief  验证信息提示
  */
const char *AS608_EnsureMessage(u8 ensure);

#endif

