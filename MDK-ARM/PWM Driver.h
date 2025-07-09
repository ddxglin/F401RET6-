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
/* --------------------- 类型定义 --------------------- */
// 电机方向类型（复用GPIO状态，避免类型错误）
typedef GPIO_PinState MotorDirection;
#define MOTOR_FORWARD  GPIO_PIN_RESET  // 正转（根据硬件接线调整）
#define MOTOR_BACKWARD GPIO_PIN_SET    // 反转

/* --------------------- 函数声明 --------------------- */
void PWM_Init(void);
void Motor_SetSpeed(uint8_t motorId, uint16_t speed);
void Motor_SetDirection(uint8_t motorId, MotorDirection direction);

#endif /* PWM_Driver_H */
