
/* 包含头文件 ----------------------------------------------------------------*/
#include "ds18b20.h"

/**
 * 函数功能: 使DS18B20-DATA引脚变为上拉输入模式
 */ 
static void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 串口外设功能GPIO配置 */
    GPIO_InitStruct.Pin = DS18B20_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}

/**
 * 函数功能: 使DS18B20-DATA引脚变为推挽输出模式
 */
static void DS18B20_Mode_Out_OD(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}

/**
 * 函数功能: 主机给从机发送复位脉冲
 */
static void DS18B20_Rst(void)
{
    DS18B20_Mode_Out_OD(); /* 主机设置为推挽输出 */
    DS18B20_Dout_LOW();    /* 主机输出低电平 */
    DS18B20_Delay(750);    /* 主机至少产生480us的低电平复位信号 */
    DS18B20_Dout_HIGH();   /* 主机在产生复位信号后，需将总线拉高 */
    DS18B20_Delay(15);     /* 从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲 */
}

/**
 * 函数功能: 检测从机给主机返回的存在脉冲
 * 返 回 值: 0：成功，1：失败
 */

static uint8_t DS18B20_Presence(void)
{
    uint8_t pulse_time = 0;
    /* 主机设置为上拉输入 */
    DS18B20_Mode_IPU();

    /* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号
     * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
     */
    while (DS18B20_Data_IN() && pulse_time < 100)
    {
        pulse_time++;
        DS18B20_Delay(1);
    }
    /* 经过100us后，存在脉冲都还没有到来*/
    if (pulse_time >= 100)
        return 1;
    else
        pulse_time = 0;

    /* 存在脉冲到来，且存在的时间不能超过240us */
    while (!DS18B20_Data_IN() && pulse_time < 240)
    {
        pulse_time++;
        DS18B20_Delay(1);
    }
    if (pulse_time >= 240)
        return 1;
    else
        return 0;
}

/**
 * 函数功能: 从DS18B20读取一个bit
 * 返 回 值: 读取到的数据
 */
static uint8_t DS18B20_ReadBit(void)
{
    uint8_t dat;

    /* 读0和读1的时间至少要大于60us */
    DS18B20_Mode_Out_OD();
    /* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
    DS18B20_Dout_LOW();
    DS18B20_Delay(10);

    /* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
    DS18B20_Mode_IPU();
    // Delay_us(2);

    if (DS18B20_Data_IN() == GPIO_PIN_SET)
        dat = 1;
    else
        dat = 0;

    /* 这个延时参数请参考时序图 */
    DS18B20_Delay(45);

    return dat;
}

/**
 * 函数功能: 从DS18B20读一个字节，低位先行
 * 返 回 值: 读到的数据
 */
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, j, dat = 0;
    for (i = 0; i < 8; i++)
    {
        j = DS18B20_ReadBit();
        dat = (dat) | (j << i);
    }
    return dat;
}

/**
 * 函数功能: 写一个字节到DS18B20，低位先行
 * 输入参数: dat：待写入数据
 */
static void DS18B20_WriteByte(uint8_t dat)
{
    uint8_t i, testb;
    DS18B20_Mode_Out_OD();
    for (i = 0; i < 8; i++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        /* 写0和写1的时间至少要大于60us */
        if (testb)
        {
            DS18B20_Dout_LOW();
            /* 1us < 这个延时 < 15us */
            DS18B20_Delay(8);

            DS18B20_Dout_HIGH();
            DS18B20_Delay(58);
        }
        else
        {
            DS18B20_Dout_LOW();
            /* 60us < Tx 0 < 120us */
            DS18B20_Delay(70);

            DS18B20_Dout_HIGH();
            /* 1us < Trec(恢复时间) < 无穷大*/
            DS18B20_Delay(2);
        }
    }
}

/**
 * 函数功能: 跳过匹配 DS18B20 ROM
 */
static void DS18B20_SkipRom(void)
{
    DS18B20_Rst();
    DS18B20_Presence();
    DS18B20_WriteByte(0XCC); /* 跳过 ROM */
}

/**
 * 函数功能: 获取 DS18B20 温度值
 * 返 回 值: 浮点型温度值
 */
float DS18B20_GetTemp_SkipRom(void)
{
    uint8_t tpmsb, tplsb;
    short s_tem;
    float f_tem;

    DS18B20_SkipRom();
    DS18B20_WriteByte(0X44); /* 开始转换 */

    DS18B20_SkipRom();
    DS18B20_WriteByte(0XBE); /* 读温度值 */

    tplsb = DS18B20_ReadByte();
    tpmsb = DS18B20_ReadByte();

    s_tem = tpmsb << 8;
    s_tem = s_tem | tplsb;

    if (s_tem < 0) /* 负温度 */
        f_tem = (~s_tem + 1) * 0.0625;
    else
        f_tem = s_tem * 0.0625;

    return f_tem;
}

/**
 * 微妙延时函数
 * 全系列通用，只需要将宏定义CPU_FREQUENCY_MHZ根据时钟主频修改即可。DS18b20
 * 系统滴答定时器是HAL库初始化的，且必须有HAL库初始化。
 */
#define CPU_FREQUENCY_MHZ (int)(HAL_RCC_GetHCLKFreq() / 1000000) // 自动获取STM32时钟主频
void DS18B20_Delay(__IO uint32_t delay)
{
    int last, curr, val;
    int temp;

    while (delay != 0)
    {
        temp = delay > 900 ? 900 : delay;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            } while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            } while ((val <= last) || (val > curr));
        }
        delay -= temp;
    }
}


/**
 * 函数功能: DS18B20 初始化函数
 * 返 回 值: 1为初始化失败，0为初始化成功
 */
uint8_t DS18B20_Init(void)
{
    DS18B20_Mode_Out_OD();  // 开漏输出模式（关键修改！）

    DS18B20_Dout_HIGH();    // 初始拉高总线

    DS18B20_Rst();          // 发送复位脉冲
    return DS18B20_Presence(); // 检测存在脉冲
}
