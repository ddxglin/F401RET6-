#ifndef __GY906_H
#define __GY906_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "i2c.h"

#define SCK_Pin   GPIO_PIN_10    // ���� SCL �� PB8��I2C1_SCL��
#define SCK_GPIO_Port  GPIOB         // SCL ��Ӧ GPIO �˿�
#define SDA_Pin   GPIO_PIN_3    // ���� SDA �� PB9��I2C1_SDA��
#define SDA_GPIO_Port  GPIOB         // SDA ��Ӧ GPIO �˿�

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t


void SMBus_StartBit(void);
void SMBus_StopBit(void);
void SMBus_SendBit(u8);
u8 SMBus_SendByte(u8);
u8 SMBus_ReceiveBit(void);
u8 SMBus_ReceiveByte(u8);
void SMBus_Delay(u32);
void SMBus_Init(void);
u16 SMBus_ReadMemory(u8, u8);
u8 PEC_Calculation(u8*);
float SMBus_ReadTemp(void); 
void Coarse_delay_us(uint32_t us);
#endif
