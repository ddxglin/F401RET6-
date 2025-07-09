
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "ds18b20.h"

/**
 * ��������: ʹDS18B20-DATA���ű�Ϊ��������ģʽ
 */ 
static void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* �������蹦��GPIO���� */
    GPIO_InitStruct.Pin = DS18B20_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}

/**
 * ��������: ʹDS18B20-DATA���ű�Ϊ�������ģʽ
 */
static void DS18B20_Mode_Out_OD(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // ��©���
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // ��������
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}

/**
 * ��������: �������ӻ����͸�λ����
 */
static void DS18B20_Rst(void)
{
    DS18B20_Mode_Out_OD(); /* ��������Ϊ������� */
    DS18B20_Dout_LOW();    /* ��������͵�ƽ */
    DS18B20_Delay(750);    /* �������ٲ���480us�ĵ͵�ƽ��λ�ź� */
    DS18B20_Dout_HIGH();   /* �����ڲ�����λ�źź��轫�������� */
    DS18B20_Delay(15);     /* �ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ���������� */
}

/**
 * ��������: ���ӻ����������صĴ�������
 * �� �� ֵ: 0���ɹ���1��ʧ��
 */

static uint8_t DS18B20_Presence(void)
{
    uint8_t pulse_time = 0;
    /* ��������Ϊ�������� */
    DS18B20_Mode_IPU();

    /* �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź�
     * �����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
     */
    while (DS18B20_Data_IN() && pulse_time < 100)
    {
        pulse_time++;
        DS18B20_Delay(1);
    }
    /* ����100us�󣬴������嶼��û�е���*/
    if (pulse_time >= 100)
        return 1;
    else
        pulse_time = 0;

    /* �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us */
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
 * ��������: ��DS18B20��ȡһ��bit
 * �� �� ֵ: ��ȡ��������
 */
static uint8_t DS18B20_ReadBit(void)
{
    uint8_t dat;

    /* ��0�Ͷ�1��ʱ������Ҫ����60us */
    DS18B20_Mode_Out_OD();
    /* ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź� */
    DS18B20_Dout_LOW();
    DS18B20_Delay(10);

    /* ���ó����룬�ͷ����ߣ����ⲿ�������轫�������� */
    DS18B20_Mode_IPU();
    // Delay_us(2);

    if (DS18B20_Data_IN() == GPIO_PIN_SET)
        dat = 1;
    else
        dat = 0;

    /* �����ʱ������ο�ʱ��ͼ */
    DS18B20_Delay(45);

    return dat;
}

/**
 * ��������: ��DS18B20��һ���ֽڣ���λ����
 * �� �� ֵ: ����������
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
 * ��������: дһ���ֽڵ�DS18B20����λ����
 * �������: dat����д������
 */
static void DS18B20_WriteByte(uint8_t dat)
{
    uint8_t i, testb;
    DS18B20_Mode_Out_OD();
    for (i = 0; i < 8; i++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        /* д0��д1��ʱ������Ҫ����60us */
        if (testb)
        {
            DS18B20_Dout_LOW();
            /* 1us < �����ʱ < 15us */
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
            /* 1us < Trec(�ָ�ʱ��) < �����*/
            DS18B20_Delay(2);
        }
    }
}

/**
 * ��������: ����ƥ�� DS18B20 ROM
 */
static void DS18B20_SkipRom(void)
{
    DS18B20_Rst();
    DS18B20_Presence();
    DS18B20_WriteByte(0XCC); /* ���� ROM */
}

/**
 * ��������: ��ȡ DS18B20 �¶�ֵ
 * �� �� ֵ: �������¶�ֵ
 */
float DS18B20_GetTemp_SkipRom(void)
{
    uint8_t tpmsb, tplsb;
    short s_tem;
    float f_tem;

    DS18B20_SkipRom();
    DS18B20_WriteByte(0X44); /* ��ʼת�� */

    DS18B20_SkipRom();
    DS18B20_WriteByte(0XBE); /* ���¶�ֵ */

    tplsb = DS18B20_ReadByte();
    tpmsb = DS18B20_ReadByte();

    s_tem = tpmsb << 8;
    s_tem = s_tem | tplsb;

    if (s_tem < 0) /* ���¶� */
        f_tem = (~s_tem + 1) * 0.0625;
    else
        f_tem = s_tem * 0.0625;

    return f_tem;
}

/**
 * ΢����ʱ����
 * ȫϵ��ͨ�ã�ֻ��Ҫ���궨��CPU_FREQUENCY_MHZ����ʱ����Ƶ�޸ļ��ɡ�DS18b20
 * ϵͳ�δ�ʱ����HAL���ʼ���ģ��ұ�����HAL���ʼ����
 */
#define CPU_FREQUENCY_MHZ (int)(HAL_RCC_GetHCLKFreq() / 1000000) // �Զ���ȡSTM32ʱ����Ƶ
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
 * ��������: DS18B20 ��ʼ������
 * �� �� ֵ: 1Ϊ��ʼ��ʧ�ܣ�0Ϊ��ʼ���ɹ�
 */
uint8_t DS18B20_Init(void)
{
    DS18B20_Mode_Out_OD();  // ��©���ģʽ���ؼ��޸ģ���

    DS18B20_Dout_HIGH();    // ��ʼ��������

    DS18B20_Rst();          // ���͸�λ����
    return DS18B20_Presence(); // ����������
}
