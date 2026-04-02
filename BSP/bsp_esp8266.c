#include "bsp_esp8266.h"
#include "bsp_delay.h"
#include "string.h"
#include <stdio.h>

// ESP8266数据帧结构体实例
ESP8266_FrameTypeDef ESP8266_Frame;

// ESP8266发送一个字节
void ESP8266_SendByte(uint8_t data) {
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}

// ESP8266发送字符串
void ESP8266_SendString(char *data) {
    while(*data) {
        ESP8266_SendByte(*data++);
    }
}

// ESP8266发送命令并等待响应
uint8_t ESP8266_SendCmd(char *cmd, char *response, uint32_t timeout_ms) {
    // 清空接收缓冲区
    memset(ESP8266_Frame.Data_RX_BUF, 0, ESP8266_RX_BUF_SIZE);
    ESP8266_Frame.InfBit.FramLength = 0;
    ESP8266_Frame.InfBit.FramFinishFlag = 0;
    
    // 发送命令
    ESP8266_SendString(cmd);
    ESP8266_SendString("\r\n");
    
    // 等待响应
    return ESP8266_WaitForResponse(response, timeout_ms);
}

// ESP8266等待响应
uint8_t ESP8266_WaitForResponse(char *response, uint32_t timeout_ms) {
    while(timeout_ms--) {
        // 检查是否收到完整数据
        if(ESP8266_Frame.InfBit.FramFinishFlag) {
            // 查找响应字符串
            if(strstr((char*)ESP8266_Frame.Data_RX_BUF, response) != NULL) {
                return 1; // 找到响应
            }
            return 0; // 未找到响应
        }
        Delay_ms(1);
    }
    
    return 0; // 超时
}

// ESP8266初始化
void ESP8266_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 配置USART2_TX (PA2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART2_RX (PA3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART2
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);
    
    // 配置中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能USART2接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // 使能USART2
    USART_Cmd(USART2, ENABLE);
    
    // 延时等待模块稳定
    Delay_ms(100);
}

// ESP8266检测
uint8_t ESP8266_Check(void) {
    return ESP8266_SendCmd("AT", "OK", 1000);
}

// 设置ESP8266工作模式
void ESP8266_SetMode(ESP8266_ModeTypeDef mode) {
    char cmd[20];
    
    switch(mode) {
        case ESP8266_MODE_STA:
            sprintf(cmd, "AT+CWMODE=1");
            break;
        case ESP8266_MODE_AP:
            sprintf(cmd, "AT+CWMODE=2");
            break;
        case ESP8266_MODE_STA_AP:
            sprintf(cmd, "AT+CWMODE=3");
            break;
        default:
            return;
    }
    
    ESP8266_SendCmd(cmd, "OK", 1000);
}

// 连接WiFi AP
uint8_t ESP8266_JoinAP(char *ssid, char *password) {
    char cmd[100];
    
    // 设置为STA模式
    ESP8266_SetMode(ESP8266_MODE_STA);
    
    // 连接WiFi
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    return ESP8266_SendCmd(cmd, "WIFI CONNECTED", 5000);
}

// 创建TCP连接
uint8_t ESP8266_CreateTCP(ESP8266_ConnIDTypeDef id, char *ip, uint16_t port) {
    char cmd[100];
    
    // 设置为单连接模式
    ESP8266_SendCmd("AT+CIPMUX=0", "OK", 1000);
    
    // 创建TCP连接
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);
    return ESP8266_SendCmd(cmd, "CONNECT", 3000);
}

// 发送数据
uint8_t ESP8266_SendData(ESP8266_ConnIDTypeDef id, uint8_t *data, uint16_t length) {
    char cmd[50];
    
    // 发送数据前先发送长度
    sprintf(cmd, "AT+CIPSEND=%d", length);
    if(!ESP8266_SendCmd(cmd, ">", 1000)) {
        return 0;
    }
    
    // 发送数据
    for(uint16_t i = 0; i < length; i++) {
        ESP8266_SendByte(data[i]);
    }
    
    // 等待发送完成
    return ESP8266_WaitForResponse("SEND OK", 2000);
}

// USART2中断服务函数
void USART2_IRQHandler(void) {
    uint8_t data;
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        // 读取接收到的数据
        data = USART_ReceiveData(USART2);
        
        // 存储数据到缓冲区
        if(ESP8266_Frame.InfBit.FramLength < ESP8266_RX_BUF_SIZE - 1) {
            ESP8266_Frame.Data_RX_BUF[ESP8266_Frame.InfBit.FramLength++] = data;
        }
        
        // 检查是否收到回车换行符，表示一帧数据结束
        if(data == '\n' && ESP8266_Frame.InfBit.FramLength >= 2) {
            if(ESP8266_Frame.Data_RX_BUF[ESP8266_Frame.InfBit.FramLength - 2] == '\r') {
                ESP8266_Frame.Data_RX_BUF[ESP8266_Frame.InfBit.FramLength - 2] = '\0'; // 替换\r为字符串结束符
                ESP8266_Frame.InfBit.FramLength -= 2; // 减去\r\n的长度
                ESP8266_Frame.InfBit.FramFinishFlag = 1; // 设置数据帧完成标志
            }
        }
        
        // 清除中断标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}


