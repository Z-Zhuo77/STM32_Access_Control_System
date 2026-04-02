#include "stm32f10x.h"
#include "BSP.h"

// 中文文本（GB2312编码）
uint8_t welcome_text[] = {0xBB,0xB6,0xD3,0xAD,0xCA,0xB9,0xD3,0xC3,0xC3,0xC5,0xBD,0xFB,0xCF,0xB5,0xCD,0xB3,0x00}; // 欢迎使用门禁系统
uint8_t key1_text[] = {0xB0,0xB4,0xBC,0xFC,0xD2,0xBB,0xB1,0xBB,0xB0,0xB4,0xCF,0xC2,0x00}; // 按键一被按下
uint8_t weight_text[] = {0xB5,0xB1,0xC7,0xB0,0xD6,0xD3,0xC1,0xBF,0x00}; // 当前重量
uint8_t raw_data_text[] = {0xD4,0xAD,0xCA,0xBC,0xCA,0xF3,0xBE,0xDD,0xD2,0xD1,0xCF,0xD6,0xCA,0xBE,0x00}; // 原始数据已显示
uint8_t clear_text[] = {0xC6,0xF8,0xC4,0xBF,0xD2,0xD1,0xC7,0xEC,0x96,0x80,0x00}; // 屏幕已清除
uint8_t beep_text[] = {0xB3,0xA4,0xC3,0xF9,0xB2,0xE2,0xCA,0xD4,0xCD,0xF6,0xBC,0xC6,0x00}; // 长鸣测试完成
uint8_t weight_done_text[] = {0xD6,0xD3,0xC1,0xBF,0xD6,0xC6,0xCA,0xBE,0xCD,0xF6,0xBC,0xC6,0x00}; // 重量显示完成
uint8_t esp8266_ok_text[] = {0xC9,0xCB,0xD5,0xD0,0xB2,0xE2,0xB3,0xC9,0xB9,0xA6,0xB9,0xA6,0x00}; // ESP8266检测成功
uint8_t esp8266_error_text[] = {0xC9,0xCB,0xD5,0xD0,0xB2,0xE2,0xB3,0xC9,0xCA,0xA7,0xB0,0xDC,0x00}; // ESP8266检测失败
uint8_t wifi_ok_text[] = {0xCD,0xF8,0xB7,0xC8,0xC1,0xAC,0xBD,0xD3,0xB9,0xA6,0xB9,0xA6,0x00}; // WiFi连接成功
uint8_t wifi_error_text[] = {0xCD,0xF8,0xB7,0xC8,0xC1,0xAC,0xBD,0xD3,0xCA,0xA7,0xB0,0xDC,0x00}; // WiFi连接失败
uint8_t fp_init_ok_text[] = {0xD6,0xB8,0xBC,0xC6,0xC4,0xA3,0xB9,0xA6,0xB9,0xA6,0x00}; // 指纹初始化成功
uint8_t fp_init_error_text[] = {0xD6,0xB8,0xBC,0xC6,0xC4,0xA3,0xCA,0xA7,0xB0,0xDC,0x00}; // 指纹初始化失败
uint8_t fp_place_text[] = {0xC7,0xB0,0xCF,0xD6,0xB8,0xBC,0xC6,0x00}; // 请放手指
uint8_t fp_remove_text[] = {0xC7,0xB0,0xC8,0xB4,0xD7,0xA2,0xB8,0xBC,0xC6,0x00}; // 请移开手指
uint8_t fp_register_ok_text[] = {0xD6,0xB8,0xBC,0xC6,0xD7,0xA2,0xB4,0xAB,0xB9,0xA6,0xB9,0xA6,0x00}; // 指纹注册成功
uint8_t fp_register_error_text[] = {0xD6,0xB8,0xBC,0xC6,0xD7,0xA2,0xB4,0xAB,0xCA,0xA7,0xB0,0xDC,0x00}; // 指纹注册失败
uint8_t fp_verify_ok_text[] = {0xD6,0xB8,0xBC,0xC6,0xD1,0xE9,0xD6,0xA4,0xB9,0xA6,0xB9,0xA6,0x00}; // 指纹验证成功
uint8_t fp_verify_error_text[] = {0xD6,0xB8,0xBC,0xC6,0xD1,0xE9,0xD6,0xA4,0xCA,0xA7,0xB0,0xDC,0x00}; // 指纹验证失败
uint8_t fp_search_ok_text[] = {0xD6,0xB8,0xBC,0xC6,0xCB,0xF5,0xB9,0xA6,0xB9,0xA6,0x00}; // 指纹搜索成功
uint8_t fp_search_error_text[] = {0xD6,0xB8,0xBC,0xC6,0xCB,0xF5,0xCA,0xA7,0xB0,0xDC,0x00}; // 指纹搜索失败
uint8_t fp_empty_ok_text[] = {0xB8,0xBC,0xC6,0xC0,0xB0,0xC8,0xB4,0xD7,0xA2,0xB9,0xA6,0xB9,0xA6,0x00}; // 指纹清空成功
uint8_t fp_empty_error_text[] = {0xB8,0xBC,0xC6,0xC0,0xB0,0xC8,0xB4,0xD7,0xA2,0xCA,0xA7,0xB0,0xDC,0x00}; // 指纹清空失败

// 指纹相关变量
SysPara fp_para;
SearchResult fp_result;

int main(void)
{
    uint8_t key_value;
    
    // 初始化延时
    Delay_Init();
    
    // 初始化所有外设
    BEEP_Init();
    OLED_Init();
    KEY_Init();
    HX711_Init();
    SYN6288_Init();
    ESP8266_Init();
    AS608_Init();
    
    // 蜂鸣器提示
    BEEP_On();
    Delay_ms(200);
    BEEP_Off();
    
    // HX711去皮
    HX711_GetZero();
    
    // 测试提示
    OLED_ShowString(2, 1, "System Ready!   ");
    
    // SYN6288测试：播放欢迎语
    SYN6288_PlayText(SYN6288_MUSIC_1, welcome_text, sizeof(welcome_text) - 1);
    Delay_ms(3000);
    
    // ESP8266测试
    OLED_Clear();
    //OLED_ShowString(1, 1, "ESP8266 Test...");
    
    // 检测ESP8266
//    if(ESP8266_Check()) {
//        OLED_ShowString(2, 1, "ESP8266 OK");
//        SYN6288_PlayText(SYN6288_MUSIC_2, esp8266_ok_text, sizeof(esp8266_ok_text) - 1);
//    } else {
//        OLED_ShowString(2, 1, "ESP8266 Error");
//        SYN6288_PlayText(SYN6288_MUSIC_2, esp8266_error_text, sizeof(esp8266_error_text) - 1);
//    }
//    Delay_ms(2000);
//    
//    // 连接WiFi
//    OLED_ShowString(3, 1, "Connecting WiFi...");
//    if(ESP8266_JoinAP("YourWiFiSSID", "YourWiFiPassword")) {
//        OLED_ShowString(3, 1, "WiFi Connected");
//        SYN6288_PlayText(SYN6288_MUSIC_3, wifi_ok_text, sizeof(wifi_ok_text) - 1);
//    } else {
//        OLED_ShowString(3, 1, "WiFi Failed");
//        SYN6288_PlayText(SYN6288_MUSIC_3, wifi_error_text, sizeof(wifi_error_text) - 1);
//    }
//    Delay_ms(2000);
    
    // 指纹模块测试
    OLED_Clear();
    OLED_ShowString(1, 1, "Fingerprint Test...");
    
    // 验证密码
    if(AS608_VerifyPassword() == 0x00) {
        OLED_ShowString(2, 1, "FP Password OK");
        SYN6288_PlayText(SYN6288_MUSIC_2, fp_init_ok_text, sizeof(fp_init_ok_text) - 1);
        
        // 读取系统参数
        if(AS608_ReadSysPara(&fp_para) == 0x00) {
            OLED_ShowString(3, 1, "FP Params Read OK");
            Delay_ms(1000);
            
            OLED_Clear();
            OLED_ShowString(1, 1, "FP Capacity: ");
            OLED_ShowNum(1, 14, fp_para.PS_max, 3);
            OLED_ShowString(2, 1, "Security Level: ");
            OLED_ShowNum(2, 16, fp_para.PS_level, 1);
            OLED_ShowString(3, 1, "Baud Rate: ");
            OLED_ShowNum(3, 12, fp_para.PS_N * 9600, 5);
            Delay_ms(2000);
        }
    } else {
        OLED_ShowString(2, 1, "FP Password Error");
        SYN6288_PlayText(SYN6288_MUSIC_2, fp_init_error_text, sizeof(fp_init_error_text) - 1);
    }
    Delay_ms(2000);
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Key Test");
    OLED_ShowString(2, 1, "Press keys...");
    OLED_ShowString(3, 1, "Key: ");
    OLED_ShowString(4, 1, "Action: None");
    
    while(1)
    {
        // 读取按键
        key_value = KEY_Scan();
        
        if(key_value != KEY_NONE)
        {
            // 显示按键值
            OLED_ShowString(3, 6, "     ");
            switch(key_value)
            {
                case KEY_0:
                    OLED_ShowString(3, 6, "0");
                    break;
                case KEY_1:
                    OLED_ShowString(3, 6, "1");
                    break;
                case KEY_2:
                    OLED_ShowString(3, 6, "2");
                    break;
                case KEY_3:
                    OLED_ShowString(3, 6, "3");
                    break;
                case KEY_4:
                    OLED_ShowString(3, 6, "4");
                    break;
                case KEY_5:
                    OLED_ShowString(3, 6, "5");
                    break;
                case KEY_6:
                    OLED_ShowString(3, 6, "6");
                    break;
                case KEY_7:
                    OLED_ShowString(3, 6, "7");
                    break;
                case KEY_8:
                    OLED_ShowString(3, 6, "8");
                    break;
                case KEY_9:
                    OLED_ShowString(3, 6, "9");
                    break;
                case KEY_A:
                    OLED_ShowString(3, 6, "A");
                    break;
                case KEY_B:
                    OLED_ShowString(3, 6, "B");
                    break;
                case KEY_C:
                    OLED_ShowString(3, 6, "C");
                    break;
                case KEY_D:
                    OLED_ShowString(3, 6, "D");
                    break;
                case KEY_STAR:
                    OLED_ShowString(3, 6, "*");
                    break;
                case KEY_POUND:
                    OLED_ShowString(3, 6, "#");
                    break;
                default:
                    OLED_ShowString(3, 6, "?");
                    break;
            }
            
            // 根据按键做不同动作
            switch(key_value)
            {
                case KEY_1:
                    OLED_ShowString(4, 1, "Action: Beep");
                    BEEP_On();
                    Delay_ms(500);
                    BEEP_Off();
                    SYN6288_PlayText(SYN6288_MUSIC_2, key1_text, sizeof(key1_text) - 1);
                    break;
                case KEY_2:
                    {
                        float weight = HX711_GetWeight();
                        OLED_ShowString(4, 1, "Weight: ");
                        OLED_ShowNum(4, 8, (uint32_t)weight, 3);
                        OLED_ShowString(4, 11, "g");
                        SYN6288_PlayText(SYN6288_MUSIC_3, weight_text, sizeof(weight_text) - 1);
                    }
                    break;
                case KEY_3:
                    {
                        uint32_t raw_data = HX711_Read();
                        OLED_ShowString(4, 1, "Raw: ");
                        OLED_ShowNum(4, 6, raw_data, 8);
                        SYN6288_PlayText(SYN6288_MUSIC_4, raw_data_text, sizeof(raw_data_text) - 1);
                    }
                    break;
                case KEY_4:
                    OLED_ShowString(4, 1, "Action: Clear");
                    Delay_ms(100);
                    OLED_Clear();
                    Delay_ms(100);
                    OLED_ShowString(1, 1, "Key Test");
                    OLED_ShowString(2, 1, "Press keys...");
                    OLED_ShowString(3, 1, "Key: ");
                    OLED_ShowString(4, 1, "Action: None");
                    SYN6288_PlayText(SYN6288_MUSIC_5, clear_text, sizeof(clear_text) - 1);
                    break;
                case KEY_5:
                    // 指纹注册
                    OLED_Clear();
                    OLED_ShowString(1, 1, "FP Register");
                    OLED_ShowString(2, 1, "Place Finger");
                    SYN6288_PlayText(SYN6288_MUSIC_2, fp_place_text, sizeof(fp_place_text) - 1);
                    
                    // 第一次采集
                    if(AS608_GetImage() == 0x00) {
                        OLED_ShowString(3, 1, "Image OK");
                        if(AS608_GenChar(CharBuffer1) == 0x00) {
                            OLED_ShowString(4, 1, "First OK");
                            Delay_ms(1000);
                            
                            OLED_ShowString(2, 1, "Place Again");
                            SYN6288_PlayText(SYN6288_MUSIC_3, fp_place_text, sizeof(fp_place_text) - 1);
                            
                            // 第二次采集
                            if(AS608_GetImage() == 0x00) {
                                if(AS608_GenChar(CharBuffer2) == 0x00) {
                                    OLED_ShowString(3, 1, "Second OK");
                                    
                                    // 合并模板
                                    if(AS608_RegModel() == 0x00) {
                                        OLED_ShowString(4, 1, "Merge OK");
                                        Delay_ms(500);
                                        
                                        // 存储模板
                                        if(AS608_StoreChar(CharBuffer2, 0) == 0x00) {
                                            OLED_Clear();
                                            OLED_ShowString(1, 1, "Register Success");
                                            SYN6288_PlayText(SYN6288_MUSIC_1, fp_register_ok_text, sizeof(fp_register_ok_text) - 1);
                                        } else {
                                            OLED_ShowString(4, 1, "Store Error");
                                            SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                                        }
                                    } else {
                                        OLED_ShowString(4, 1, "Merge Error");
                                        SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                                    }
                                } else {
                                    OLED_ShowString(3, 1, "Second Error");
                                    SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                                }
                            } else {
                                OLED_ShowString(2, 1, "Image Error");
                                SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                            }
                        } else {
                            OLED_ShowString(3, 1, "GenChar Error");
                            SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                        }
                    } else {
                        OLED_ShowString(2, 1, "Image Error");
                        SYN6288_PlayText(SYN6288_MUSIC_2, fp_register_error_text, sizeof(fp_register_error_text) - 1);
                    }
                    Delay_ms(2000);
                    
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Key Test");
                    OLED_ShowString(2, 1, "Press keys...");
                    OLED_ShowString(3, 1, "Key: ");
                    OLED_ShowString(4, 1, "Action: None");
                    break;
                case KEY_6:
                    // 指纹验证
                    OLED_Clear();
                    OLED_ShowString(1, 1, "FP Verify");
                    OLED_ShowString(2, 1, "Place Finger");
                    SYN6288_PlayText(SYN6288_MUSIC_2, fp_place_text, sizeof(fp_place_text) - 1);
                    
                    if(AS608_GetImage() == 0x00) {
                        OLED_ShowString(3, 1, "Image OK");
                        if(AS608_GenChar(CharBuffer1) == 0x00) {
                            OLED_ShowString(4, 1, "GenChar OK");
                            Delay_ms(500);
                            
                            if(AS608_HighSpeedSearch(CharBuffer1, 0, 100, &fp_result) == 0x00) {
                                OLED_Clear();
                                OLED_ShowString(1, 1, "Verify Success");
                                OLED_ShowString(2, 1, "ID: ");
                                OLED_ShowNum(2, 4, fp_result.pageID, 3);
                                OLED_ShowString(3, 1, "Score: ");
                                OLED_ShowNum(3, 7, fp_result.mathscore, 4);
                                SYN6288_PlayText(SYN6288_MUSIC_1, fp_verify_ok_text, sizeof(fp_verify_ok_text) - 1);
                            } else {
                                OLED_Clear();
                                OLED_ShowString(1, 1, "Verify Failed");
                                SYN6288_PlayText(SYN6288_MUSIC_2, fp_verify_error_text, sizeof(fp_verify_error_text) - 1);
                            }
                        } else {
                            OLED_ShowString(4, 1, "GenChar Error");
                        }
                    } else {
                        OLED_ShowString(3, 1, "Image Error");
                    }
                    Delay_ms(2000);
                    
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Key Test");
                    OLED_ShowString(2, 1, "Press keys...");
                    OLED_ShowString(3, 1, "Key: ");
                    OLED_ShowString(4, 1, "Action: None");
                    break;
                case KEY_7:
                    // 清空指纹库
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Clear FP Library");
                    OLED_ShowString(2, 1, "Are you sure?");
                    OLED_ShowString(3, 1, "Press 7 again");
                    
                    // 等待按键确认
                    while(KEY_Scan() != KEY_7);
                    
                    if(AS608_Empty() == 0x00) {
                        OLED_Clear();
                        OLED_ShowString(1, 1, "Clear Success");
                        SYN6288_PlayText(SYN6288_MUSIC_1, fp_empty_ok_text, sizeof(fp_empty_ok_text) - 1);
                    } else {
                        OLED_Clear();
                        OLED_ShowString(1, 1, "Clear Failed");
                        SYN6288_PlayText(SYN6288_MUSIC_2, fp_empty_error_text, sizeof(fp_empty_error_text) - 1);
                    }
                    Delay_ms(2000);
                    
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Key Test");
                    OLED_ShowString(2, 1, "Press keys...");
                    OLED_ShowString(3, 1, "Key: ");
                    OLED_ShowString(4, 1, "Action: None");
                    break;
                case KEY_A:
                    OLED_ShowString(4, 1, "Action: Beep Long");
                    BEEP_On();
                    Delay_ms(1000);
                    BEEP_Off();
                    SYN6288_PlayText(SYN6288_MUSIC_6, beep_text, sizeof(beep_text) - 1);
                    break;
                case KEY_B:
                    OLED_ShowString(4, 1, "Action: Show Weight");
                    {
                        float weight = HX711_GetWeight();
                        OLED_ShowString(4, 1, "Weight: ");
                        OLED_ShowNum(4, 8, (uint32_t)weight, 3);
                        OLED_ShowString(4, 11, "g");
                        SYN6288_PlayText(SYN6288_MUSIC_7, weight_done_text, sizeof(weight_done_text) - 1);
                    }
                    break;
                case KEY_C:
                    // 读取有效模板数量
                    {
                        u16 valid_num;
                        if(AS608_ValidTempleteNum(&valid_num) == 0x00) {
                            OLED_Clear();
                            OLED_ShowString(1, 1, "Valid Templates:");
                            OLED_ShowNum(2, 1, valid_num, 3);
                            Delay_ms(2000);
                            
                            OLED_Clear();
                            OLED_ShowString(1, 1, "Key Test");
                            OLED_ShowString(2, 1, "Press keys...");
                            OLED_ShowString(3, 1, "Key: ");
                            OLED_ShowString(4, 1, "Action: None");
                        }
                    }
                    break;
                default:
                    OLED_ShowString(4, 1, "Action: None");
                    break;
            }
            
            // 延时消抖
            Delay_ms(200);
        }
    }
}