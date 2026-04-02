#include "bsp_syn6288.h"
#include "bsp_delay.h"
#include "string.h"

// 发送一个字节
void SYN6288_SendByte(uint8_t data)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, data);
}

// 发送字符串
void SYN6288_SendString(uint8_t *data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        SYN6288_SendByte(*data++);
    }
}

// 发送帧信息
void SYN6288_PlayText(uint8_t music, uint8_t *text, uint8_t length)
{
    uint8_t frame[100];
    uint8_t ecc = 0;
    uint8_t i = 0;
    
    // 帧头信息
    frame[0] = 0xFD; // 帧头FD
    frame[1] = 0x00; // 数据长度高位
    frame[2] = length + 3; // 数据长度低位
    frame[3] = 0x01; // 命令类型：合成播放
    frame[4] = 0x01 | (music << 4); // 命令参数：音乐类型
    
    // 计算校验和
    for(i = 0; i < 5; i++)
    {
        ecc ^= frame[i];
    }
    
    for(i = 0; i < length; i++)
    {
        ecc ^= text[i];
    }
    
    // 填充文本数据
    for(i = 0; i < length; i++)
    {
        frame[5 + i] = text[i];
    }
    frame[5 + length] = ecc;
    
    // 发送数据
    SYN6288_SendString(frame, 5 + length + 1);
}

// 初始化SYN6288
void SYN6288_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    // 配置USART3_TX (PB10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置USART3_RX (PB11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置USART3
    USART_InitStructure.USART_BaudRate = 9600; // SYN6288默认波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);
    
    // 使能USART3
    USART_Cmd(USART3, ENABLE);
    
    // 延时等待模块初始化
    Delay_ms(100);
}