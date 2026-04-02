#include "bsp_as608.h"
#include "bsp_delay.h"
#include "stdio.h"
#include "string.h"

// AS608地址
uint32_t AS608Addr = 0XFFFFFFFF;

// 串口接收缓冲区
uint8_t USART1_RX_BUF[USART1_RX_BUF_SIZE];
uint16_t USART1_RX_STA = 0;

// 串口发送一个字节
static void MYUSART_SendData(u8 data)
{
    while((USART1->SR & 0X40) == 0);
    USART1->DR = data;
}

// 发送包头
static void SendHead(void)
{
    MYUSART_SendData(0xEF);
    MYUSART_SendData(0x01);
}

// 发送地址
static void SendAddr(void)
{
    MYUSART_SendData(AS608Addr >> 24);
    MYUSART_SendData(AS608Addr >> 16);
    MYUSART_SendData(AS608Addr >> 8);
    MYUSART_SendData(AS608Addr);
}

// 发送包标识
static void SendFlag(u8 flag)
{
    MYUSART_SendData(flag);
}

// 发送包长度
static void SendLength(int length)
{
    MYUSART_SendData(length >> 8);
    MYUSART_SendData(length);
}

// 发送命令码
static void Sendcmd(u8 cmd)
{
    MYUSART_SendData(cmd);
}

// 发送校验和
static void SendCheck(u16 check)
{
    MYUSART_SendData(check >> 8);
    MYUSART_SendData(check);
}

// 判断是否接收到正确的应答包
static u8 *JudgeStr(u16 waittime)
{
    char *data;
    u8 str[8];
    str[0] = 0xef;
    str[1] = 0x01;
    str[2] = AS608Addr >> 24;
    str[3] = AS608Addr >> 16;
    str[4] = AS608Addr >> 8;
    str[5] = AS608Addr;
    str[6] = 0x07;
    str[7] = '\0';
    USART1_RX_STA = 0;
    
    while(--waittime)
    {
        Delay_ms(1);
        if(USART1_RX_STA >= 12) // 至少接收到12字节数据
        {
            data = strstr((const char*)USART1_RX_BUF, (const char*)str);
            if(data)
                return (u8*)data;
        }
    }
    return 0;
}

/**
  * @brief  AS608初始化
  */
void AS608_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置TX引脚（PA9）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置RX引脚（PA10）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART1
    USART_InitStructure.USART_BaudRate = 57600; // AS608波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 配置中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  验证密码
  */
uint8_t AS608_VerifyPassword(void)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x07);
    Sendcmd(0x01);
    MYUSART_SendData(0x00);
    MYUSART_SendData(0x00);
    MYUSART_SendData(0x00);
    MYUSART_SendData(0x00);
    temp = 0x01 + 0x07 + 0x01 + 0x00 + 0x00 + 0x00 + 0x00;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  获取指纹图像
  */
uint8_t AS608_GetImage(void)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x01);
    temp = 0x01 + 0x03 + 0x01;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  将图像存入缓冲区
  * @param  buffer_id: 缓冲区ID（1或2）
  */
uint8_t AS608_GenChar(uint8_t buffer_id)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x04);
    Sendcmd(0x02);
    MYUSART_SendData(buffer_id);
    temp = 0x01 + 0x04 + 0x02 + buffer_id;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  比对两个缓冲区的指纹
  */
uint8_t AS608_Match(void)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x03);
    temp = 0x01 + 0x03 + 0x03;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  搜索指纹
  */
uint8_t AS608_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x08);
    Sendcmd(0x04);
    MYUSART_SendData(BufferID);
    MYUSART_SendData(StartPage >> 8);
    MYUSART_SendData(StartPage);
    MYUSART_SendData(PageNum >> 8);
    MYUSART_SendData(PageNum);
    temp = 0x01 + 0x08 + 0x04 + BufferID
           + (StartPage >> 8) + (u8)StartPage
           + (PageNum >> 8) + (u8)PageNum;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
    {
        ensure = data[9];
        p->pageID = (data[10] << 8) + data[11];
        p->mathscore = (data[12] << 8) + data[13];
    }
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  合并指纹模板
  */
uint8_t AS608_RegModel(void)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x05);
    temp = 0x01 + 0x03 + 0x05;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  存储模板
  */
uint8_t AS608_StoreChar(u8 BufferID, u16 PageID)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x06);
    Sendcmd(0x06);
    MYUSART_SendData(BufferID);
    MYUSART_SendData(PageID >> 8);
    MYUSART_SendData(PageID);
    temp = 0x01 + 0x06 + 0x06 + BufferID
           + (PageID >> 8) + (u8)PageID;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  删除模板
  */
uint8_t AS608_DeletChar(u16 PageID, u16 N)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x07);
    Sendcmd(0x0C);
    MYUSART_SendData(PageID >> 8);
    MYUSART_SendData(PageID);
    MYUSART_SendData(N >> 8);
    MYUSART_SendData(N);
    temp = 0x01 + 0x07 + 0x0C
           + (PageID >> 8) + (u8)PageID
           + (N >> 8) + (u8)N;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  清空指纹库
  */
uint8_t AS608_Empty(void)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x0D);
    temp = 0x01 + 0x03 + 0x0D;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  读取系统参数
  */
uint8_t AS608_ReadSysPara(SysPara *p)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x0F);
    temp = 0x01 + 0x03 + 0x0F;
    SendCheck(temp);
    data = JudgeStr(1000);
    if(data)
    {
        ensure = data[9];
        p->PS_max = (data[14] << 8) + data[15];
        p->PS_level = data[17];
        p->PS_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
        p->PS_size = data[23];
        p->PS_N = data[25];
    }
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  获取有效模板数量
  */
uint8_t AS608_ValidTempleteNum(u16 *ValidN)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x1d);
    temp = 0x01 + 0x03 + 0x1d;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
    {
        ensure = data[9];
        *ValidN = (data[10] << 8) + data[11];
    }
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  写入寄存器
  */
uint8_t AS608_WriteReg(u8 RegNum, u8 DATA)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x05);
    Sendcmd(0x0E);
    MYUSART_SendData(RegNum);
    MYUSART_SendData(DATA);
    temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  设置模块地址
  */
uint8_t AS608_SetAddr(u32 PS_addr)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x07);
    Sendcmd(0x15);
    MYUSART_SendData(PS_addr >> 24);
    MYUSART_SendData(PS_addr >> 16);
    MYUSART_SendData(PS_addr >> 8);
    MYUSART_SendData(PS_addr);
    temp = 0x01 + 0x07 + 0x15
           + (u8)(PS_addr >> 24) + (u8)(PS_addr >> 16)
           + (u8)(PS_addr >> 8) + (u8)PS_addr;
    SendCheck(temp);
    AS608Addr = PS_addr; // 保存新地址，用于下次通信
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    AS608Addr = PS_addr;
    return ensure;
}

/**
  * @brief  写入记事本
  */
uint8_t AS608_WriteNotepad(u8 NotePageNum, u8 *Byte32)
{
    u16 temp = 0;
    u8 ensure, i;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(36);
    Sendcmd(0x18);
    MYUSART_SendData(NotePageNum);
    for(i = 0; i < 32; i++)
    {
        MYUSART_SendData(Byte32[i]);
        temp += Byte32[i];
    }
    temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  读取记事本
  */
uint8_t AS608_ReadNotepad(u8 NotePageNum, u8 *Byte32)
{
    u16 temp;
    u8 ensure, i;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x04);
    Sendcmd(0x19);
    MYUSART_SendData(NotePageNum);
    temp = 0x01 + 0x04 + 0x19 + NotePageNum;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
    {
        ensure = data[9];
        for(i = 0; i < 32; i++)
        {
            Byte32[i] = data[10 + i];
        }
    }
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  高速搜索
  */
uint8_t AS608_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
    u16 temp;
    u8 ensure;
    u8 *data;
    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x08);
    Sendcmd(0x1b);
    MYUSART_SendData(BufferID);
    MYUSART_SendData(StartPage >> 8);
    MYUSART_SendData(StartPage);
    MYUSART_SendData(PageNum >> 8);
    MYUSART_SendData(PageNum);
    temp = 0x01 + 0x08 + 0x1b + BufferID
           + (StartPage >> 8) + (u8)StartPage
           + (PageNum >> 8) + (u8)PageNum;
    SendCheck(temp);
    data = JudgeStr(2000);
    if(data)
    {
        ensure = data[9];
        p->pageID = (data[10] << 8) + data[11];
        p->mathscore = (data[12] << 8) + data[13];
    }
    else
        ensure = 0xff;
    return ensure;
}

/**
  * @brief  与AS608握手
  */
uint8_t AS608_HandShake(u32 *PS_Addr)
{
    SendHead();
    SendAddr();
    MYUSART_SendData(0X01);
    MYUSART_SendData(0X00);
    MYUSART_SendData(0X00);
    Delay_ms(200);
    if(USART1_RX_STA >= 12)
    {
        if(// 判断是否是模块返回的应答包
            USART1_RX_BUF[0] == 0XEF
            && USART1_RX_BUF[1] == 0X01
            && USART1_RX_BUF[6] == 0X07
        )
        {
            *PS_Addr = (USART1_RX_BUF[2] << 24) + (USART1_RX_BUF[3] << 16)
                       + (USART1_RX_BUF[4] << 8) + (USART1_RX_BUF[5]);
            USART1_RX_STA = 0;
            return 0;
        }
        USART1_RX_STA = 0;
    }
    return 1;
}

/**
  * @brief  验证信息提示
  */
const char *AS608_EnsureMessage(u8 ensure)
{
    const char *p;
    switch(ensure)
    {
    case 0x00:
        p = "       OK       ";
        break;
    case 0x01:
        p = " 数据包接收错误 ";
        break;
    case 0x02:
        p = "模块无此命令";
        break;
    case 0x03:
        p = "采集指纹图像失败";
        break;
    case 0x04:
        p = " 指纹太淡或太浅 ";
        break;
    case 0x05:
        p = " 指纹太潮湿或太干 ";
        break;
    case 0x06:
        p = " 指纹图像太模糊 ";
        break;
    case 0x07:
        p = " 指纹图像太混乱 ";
        break;
    case 0x08:
        p = " 指纹不匹配 ";
        break;
    case 0x09:
        p = " 没有发现指纹 ";
        break;
    case 0x0a:
        p = " 合并失败 ";
        break;
    case 0x0b:
        p = "地址号超出范围";
        break;
    case 0x10:
        p = " 删除模板失败 ";
        break;
    case 0x11:
        p = " 清空指纹库失败 ";
        break;
    case 0x15:
        p = "无效的图像";
        break;
    case 0x18:
        p = " 写FLASH失败 ";
        break;
    case 0x19:
        p = " 未定义错误 ";
        break;
    case 0x1a:
        p = " 无效的寄存器 ";
        break;
    case 0x1b:
        p = " 寄存器数据错误 ";
        break;
    case 0x1c:
        p = " 页码超出范围 ";
        break;
    case 0x1f:
        p = " 指纹库满 ";
        break;
    case 0x20:
        p = " 地址冲突 ";
        break;
    default:
        p = " 未知错误码 ";
        break;
    }
    return p;
}

