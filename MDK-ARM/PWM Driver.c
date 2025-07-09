#include "PWM Driver.h"
// ����������ö��


/**
 * @brief ���õ���ٶȣ�PWMռ�ձȣ�
 * @param motorId ���ID��1��2��
 * @param speed ռ�ձȣ�0-1000��Ӧ0%-100%��
 */
void Motor_SetSpeed(uint8_t motorId, uint16_t speed) {
  if (motorId == 1) {
    // ���1��TIM2_CH1��PA0��
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
  } else if (motorId == 2) {
    // ���2��TIM1_CH2��PA9��
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
  }
}

/**
 * @brief ���õ������
 * @param motorId ���ID��1��2��
 * @param direction ����MOTOR_FORWARD��MOTOR_BACKWARD��
 */
void Motor_SetDirection(uint8_t motorId, MotorDirection direction) {
  if (motorId == 1) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, direction);  // ���1������ƣ�PC1��
  } else if (motorId == 2) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, direction);  // ���2������ƣ�PA4��
  }
}
/* USER CODE END 0 */
