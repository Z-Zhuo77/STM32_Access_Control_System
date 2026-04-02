#include "bsp_key.h"
#include "bsp_delay.h"

/*
 * 函数名：KEY_Init
 * 功能：初始化矩阵按键GPIO
 * 参数：无
 * 返回值：无
 */
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 行引脚配置为推挽输出
    GPIO_InitStructure.GPIO_Pin = KEY_ROW1_PIN | KEY_ROW2_PIN | KEY_ROW3_PIN | KEY_ROW4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_ROW_PORT, &GPIO_InitStructure);
    
    // 列引脚配置为上拉输入
    GPIO_InitStructure.GPIO_Pin = KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY_COL_PORT, &GPIO_InitStructure);
    
    // 所有行引脚置高
    GPIO_SetBits(KEY_ROW_PORT, KEY_ROW1_PIN | KEY_ROW2_PIN | KEY_ROW3_PIN | KEY_ROW4_PIN);
}

/*
 * 函数名：KEY_Scan
 * 功能：扫描矩阵按键，返回按键值
 * 参数：无
 * 返回值：按键值（KEY_NONE表示无按键按下）
 */
uint8_t KEY_Scan(void)
{
    uint8_t row, col;
    uint8_t key_value = KEY_NONE;
    
    // 依次扫描每一行
    for (row = 0; row < 4; row++)
    {
        // 先将所有行置高
        GPIO_SetBits(KEY_ROW_PORT, KEY_ROW1_PIN | KEY_ROW2_PIN | KEY_ROW3_PIN | KEY_ROW4_PIN);
        
        // 当前行置低
        switch (row)
        {
            case 0: GPIO_ResetBits(KEY_ROW_PORT, KEY_ROW1_PIN); break;
            case 1: GPIO_ResetBits(KEY_ROW_PORT, KEY_ROW2_PIN); break;
            case 2: GPIO_ResetBits(KEY_ROW_PORT, KEY_ROW3_PIN); break;
            case 3: GPIO_ResetBits(KEY_ROW_PORT, KEY_ROW4_PIN); break;
        }
        
        Delay_us(10); // 延时消抖
        
        // 检测列电平
        if ((GPIO_ReadInputData(KEY_COL_PORT) & (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN)) != (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN))
        {
            // 有按键按下，延时消抖
            Delay_ms(20);
            
            // 再次检测
            if ((GPIO_ReadInputData(KEY_COL_PORT) & (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN)) != (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN))
            {
                // 确定按下的列
                if (!(GPIO_ReadInputDataBit(KEY_COL_PORT, KEY_COL1_PIN))) col = 0;
                else if (!(GPIO_ReadInputDataBit(KEY_COL_PORT, KEY_COL2_PIN))) col = 1;
                else if (!(GPIO_ReadInputDataBit(KEY_COL_PORT, KEY_COL3_PIN))) col = 2;
                else if (!(GPIO_ReadInputDataBit(KEY_COL_PORT, KEY_COL4_PIN))) col = 3;
                else return KEY_NONE;
                
                // 计算按键值
                switch (row)
                {
                    case 0: // 第一行
                        switch (col)
                        {
                            case 0: key_value = KEY_1; break;
                            case 1: key_value = KEY_2; break;
                            case 2: key_value = KEY_3; break;
                            case 3: key_value = KEY_A; break;
                        }
                        break;
                    case 1: // 第二行
                        switch (col)
                        {
                            case 0: key_value = KEY_4; break;
                            case 1: key_value = KEY_5; break;
                            case 2: key_value = KEY_6; break;
                            case 3: key_value = KEY_B; break;
                        }
                        break;
                    case 2: // 第三行
                        switch (col)
                        {
                            case 0: key_value = KEY_7; break;
                            case 1: key_value = KEY_8; break;
                            case 2: key_value = KEY_9; break;
                            case 3: key_value = KEY_C; break;
                        }
                        break;
                    case 3: // 第四行
                        switch (col)
                        {
                            case 0: key_value = KEY_STAR; break;
                            case 1: key_value = KEY_0; break;
                            case 2: key_value = KEY_POUND; break;
                            case 3: key_value = KEY_D; break;
                        }
                        break;
                }
                
                // 等待按键释放
                while ((GPIO_ReadInputData(KEY_COL_PORT) & (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN)) != (KEY_COL1_PIN | KEY_COL2_PIN | KEY_COL3_PIN | KEY_COL4_PIN));
                
                // 延时消抖
                Delay_ms(20);
                
                return key_value;
            }
        }
    }
    
    return KEY_NONE;
}

/*
 * 函数名：KEY_GetChar
 * 功能：将按键值转换为对应的字符
 * 参数：key_value - 按键值
 * 返回值：对应的字符（ASCII码）
 */
uint8_t KEY_GetChar(uint8_t key_value)
{
    switch (key_value)
    {
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_A: return 'A';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_B: return 'B';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';
        case KEY_C: return 'C';
        case KEY_STAR: return '*';
        case KEY_0: return '0';
        case KEY_POUND: return '#';
        case KEY_D: return 'D';
        default: return 0;
    }
}

