#ifndef __DS18B20_H
#define __DS18B20_H

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "main.h"

/* ���Ų��������궨�� --------------------------------------------------------*/
#define DS18B20_Dout_LOW()               HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET)
#define DS18B20_Dout_HIGH()              HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET)
#define DS18B20_Data_IN()                HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)

/* �������� ------------------------------------------------------------------*/
void 	DS18B20_Delay(__IO uint32_t delay);
float   DS18B20_GetTemp_SkipRom(void);
uint8_t DS18B20_Init(void);

#endif
