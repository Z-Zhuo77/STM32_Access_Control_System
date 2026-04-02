#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#pragma anon_unions
#include "stm32f10x.h"

// ESP8266网络模式定义
typedef enum {
    ESP8266_MODE_STA,      // 客户端模式
    ESP8266_MODE_AP,       // 接入点模式
    ESP8266_MODE_STA_AP    // 客户端+接入点模式
} ESP8266_ModeTypeDef;

// ESP8266网络协议定义
typedef enum {
    ESP8266_PROTO_TCP,     // TCP协议
    ESP8266_PROTO_UDP      // UDP协议
} ESP8266_ProtoTypeDef;

// ESP8266连接ID定义
typedef enum {
    ESP8266_CONN_ID_0 = 0,
    ESP8266_CONN_ID_1 = 1,
    ESP8266_CONN_ID_2 = 2,
    ESP8266_CONN_ID_3 = 3,
    ESP8266_CONN_ID_4 = 4,
    ESP8266_CONN_ID_SINGLE = 5
} ESP8266_ConnIDTypeDef;

// ESP8266加密模式定义
typedef enum {
    ESP8266_ENCRYPT_OPEN = 0,     // 无加密
    ESP8266_ENCRYPT_WEP = 1,      // WEP加密
    ESP8266_ENCRYPT_WPA_PSK = 2,  // WPA-PSK加密
    ESP8266_ENCRYPT_WPA2_PSK = 3, // WPA2-PSK加密
    ESP8266_ENCRYPT_WPA_WPA2 = 4  // WPA-WPA2混合加密
} ESP8266_EncryptTypeDef;

// 缓冲区大小定义
#define ESP8266_RX_BUF_SIZE 1024

// ESP8266数据帧结构体
typedef struct {
    uint8_t Data_RX_BUF[ESP8266_RX_BUF_SIZE];
    union {
        uint16_t InfAll;
        struct {
            uint16_t FramLength : 15;       // 数据帧长度
            uint16_t FramFinishFlag : 1;    // 数据帧完成标志
        } InfBit;
    };
} ESP8266_FrameTypeDef;

// 外部变量声明
extern ESP8266_FrameTypeDef ESP8266_Frame;

// 函数声明
void ESP8266_Init(void);
uint8_t ESP8266_Check(void);
void ESP8266_SetMode(ESP8266_ModeTypeDef mode);
uint8_t ESP8266_JoinAP(char *ssid, char *password);
uint8_t ESP8266_CreateTCP(ESP8266_ConnIDTypeDef id, char *ip, uint16_t port);
uint8_t ESP8266_SendData(ESP8266_ConnIDTypeDef id, uint8_t *data, uint16_t length);
uint8_t ESP8266_WaitForResponse(char *response, uint32_t timeout_ms);

#endif


