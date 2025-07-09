#ifndef PWM_Driver_H
#define PWM_Driver_H

#include "stm32f4xx_hal.h"
#include "tim.h"
#include "main.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx_hal_gpio.h"
/* --------------------- ���Ͷ��� --------------------- */
// ����������ͣ�����GPIO״̬���������ʹ���
typedef GPIO_PinState MotorDirection;
#define MOTOR_FORWARD  GPIO_PIN_RESET  // ��ת������Ӳ�����ߵ�����
#define MOTOR_BACKWARD GPIO_PIN_SET    // ��ת

/* --------------------- �������� --------------------- */
void PWM_Init(void);
void Motor_SetSpeed(uint8_t motorId, uint16_t speed);
void Motor_SetDirection(uint8_t motorId, MotorDirection direction);

#endif /* PWM_Driver_H */
