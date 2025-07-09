/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "OLED_Data.h"
#include "GY906.h"
#include "mpu6050.h"
#include "PWM Driver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_MAX_VALUE           4095  // 12λADC���ֵ
#define VOLTAGE_REF             3.3f  // �ο���ѹ(V)
#define UPDATE_INTERVAL_MS      50    // OLED���¼��(ms)
/* ���Ϳ��Ʋ�����������ڴ˴���ȷ���������������ã�*/
#define HYSTERESIS_WIDTH 2.0f    // ���Ϳ�ȣ�cm��������仯������ֵ���л�ģʽ
#define CENTER_DISTANCE 25.0f    // Ŀ�����ľ��루cm��
#define MIN_DISTANCE (CENTER_DISTANCE - HYSTERESIS_WIDTH/2)  // ��С��ֵ
#define MAX_DISTANCE (CENTER_DISTANCE + HYSTERESIS_WIDTH/2)  // �����ֵ
#define FILTER_FACTOR 0.7f       // �˲�ϵ����0.0~1.0��Խ���˲�Ч��Խǿ��
#define HEAT_THRESHOLD 30.0f     // �ȶ���ֵ�����¶ȳ�����ֵʱֹͣ
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t adcValue = 0;      // �洢ADC����ֵ
float voltage = 0.0;
float distance = 0.0;
float filtered_distance = 0.0f;  // �˲���ľ���ֵ
int current_mode = 0;            // ��ǰģʽ��0=��ʼ����1=ƫ��2=ƫ��
float temperature = 0.0;         // �洢�¶�ֵ
int heat_detected = 0;      // ��Դ����־
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DisplayTemperatureOnOLED(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
	OLED_Clear();
  SMBus_Init();
	uint16_t adcValue = 0;        // ADCֵ

	// ����PWM���
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  // ����TIM1_CH2�����2 PWM��
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  // ����TIM2_CH1�����1 PWM��

  // �������������Ϊ��ת������ʵ�ʽ��ߵ�����
  Motor_SetDirection(1, MOTOR_FORWARD);
  Motor_SetDirection(2, MOTOR_FORWARD);

  // �����������Ϊ��ͬ�ٶȣ�50%ռ�ձȣ��ɵ���0-1000��
  Motor_SetSpeed(1, 0);
  Motor_SetSpeed(2, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);  // PC7����ߵ�ƽ��ֹ��
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		
		
		
		
		
//		    DisplayTemperatureOnOLED();
//				OLED_Update();
//        HAL_Delay(500); // ÿ�����һ����ʾ
// ��ʱ����OLED��ʾ
		
		
		// ��ȡ�¶�ֵ
    temperature = SMBus_ReadTemp();

    // �����Դ��0.0���Ըĳ���Ҫ������Դ�¶�
    if (temperature > 0.0) {
        heat_detected = 1;  // ��1����true
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);  // ����������
        Motor_SetSpeed(1, 700);
        Motor_SetSpeed(2, 700);  // ��������ͬ�ٶȿ�����Դ����
    }

    // �ȶȴﵽһ���̶�ʱֹͣ
    if (heat_detected && temperature > HEAT_THRESHOLD) {
        Motor_SetSpeed(1, 0);
        Motor_SetSpeed(2, 0);  // ֹͣ���
    }
		
		if (!heat_detected) 
			{
        // ѭǽģʽ
		// ����ADCת��������ģʽ��ÿ�ζ���Ҫ������
    HAL_ADC_Start(&hadc1);
    
    // �ȴ�ת����ɣ�����鷵��ֵ
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        // ת���ɹ���ɣ���ȡADCֵ
        adcValue = HAL_ADC_GetValue(&hadc1);
        
        // �����ѹ
        voltage = (float)adcValue * 3.3f / 4095.0f;
        
        // ��ʾADC�͵�ѹ
//        OLED_ClearArea(0, 16, 128, 16);
//        OLED_Printf(0, 16, OLED_8X16, "ADC:%4d  Volt:%.2fV", adcValue, voltage);
        
        // �������
        if (voltage > 0.4f) {
            distance = 18.29f / (voltage - 0.1714f);
        } else {
            distance = 80.0f;
        }
        distance = fmaxf(10.0f, fminf(distance, 80.0f));
        
        // ��ʾ����
        OLED_ClearArea(0, 32, 128, 16);
        OLED_Printf(0, 32, OLED_8X16, "Dist:%.1fcm", distance);
				
				
				
        
// �����˲���ľ���ֵ
if (filtered_distance == 0.0f) {
    filtered_distance = distance;  // �״γ�ʼ��
} else {
    filtered_distance = FILTER_FACTOR * filtered_distance + (1.0f - FILTER_FACTOR) * distance;
}

// ���Ϳ����߼�
if (filtered_distance < MIN_DISTANCE) {
    current_mode = 1;  // ƫ��ģʽ
} else if (filtered_distance > MAX_DISTANCE) {
    current_mode = 2;  // ƫ��ģʽ
}
// ע�⣺��������13-17cm֮��ʱ�����ֵ�ǰģʽ����

// ���ݵ�ǰģʽ���õ���ٶ�
if (current_mode == 1) {
    Motor_SetSpeed(1, 400);  // �����ٶȲ��ǿת��Ч��
    Motor_SetSpeed(2, 700);
//    OLED_Printf(0, 48, OLED_8X16, "Mode: Left Turn ");
} else {
    Motor_SetSpeed(1, 700);
    Motor_SetSpeed(2, 400);  // �����ٶȲ��ǿת��Ч��
//    OLED_Printf(0, 48, OLED_8X16, "Mode: Right Turn");
}
				
				
				
				
        OLED_Update();  // ˢ����Ļ
    } else {
        // ת����ʱ���������������
        OLED_ClearArea(0, 16, 128, 16);
        OLED_Printf(0, 16, OLED_8X16, "ADC Error!       ");
        OLED_Update();
    }
	     }

    // 
       }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ��ʾ�¶ȵĺ���
void DisplayTemperatureOnOLED(void)
{
    uint16_t rawData;      // �洢ԭʼ16λ����
    char temp_str[20];

    // ��ʼ��OLED��GY906������
 

    // ��ӱ�Ҫ����ʱ��ȷ����������ʼ�����
    HAL_Delay(100); 

    // ��ȡԭʼ����
    rawData = SMBus_ReadMemory(0x00, 0x00|0x07);
    
    // ���ͨ���Ƿ�ʧ�ܣ�����ʧ��ʱ����0xFFFF��
    if (rawData == 0xFFFF) {
        sprintf(temp_str, "Temp: Error");
    } else {
        // ��ʾԭʼ16λ���ݣ�ʮ�����Ƹ�ʽ��
        sprintf(temp_str, "Raw: 0x%04X", rawData);
    }
    
    // ��OLED����ʾ�ַ���
    OLED_ShowString(0, 0, temp_str, OLED_8X16);

    // ����OLED��ʾ
    OLED_Update();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
