#include "PWM Driver.h"
// 定义电机方向枚举


/**
 * @brief 设置电机速度（PWM占空比）
 * @param motorId 电机ID（1或2）
 * @param speed 占空比（0-1000对应0%-100%）
 */
void Motor_SetSpeed(uint8_t motorId, uint16_t speed) {
  if (motorId == 1) {
    // 电机1：TIM2_CH1（PA0）
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
  } else if (motorId == 2) {
    // 电机2：TIM1_CH2（PA9）
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
  }
}

/**
 * @brief 设置电机方向
 * @param motorId 电机ID（1或2）
 * @param direction 方向（MOTOR_FORWARD或MOTOR_BACKWARD）
 */
void Motor_SetDirection(uint8_t motorId, MotorDirection direction) {
  if (motorId == 1) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, direction);  // 电机1方向控制（PC1）
  } else if (motorId == 2) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, direction);  // 电机2方向控制（PA4）
  }
}
/* USER CODE END 0 */
