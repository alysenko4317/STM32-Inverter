/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FAN_Enable_Pin GPIO_PIN_13
#define FAN_Enable_GPIO_Port GPIOC
#define FAN_Speed_1_Pin GPIO_PIN_14
#define FAN_Speed_1_GPIO_Port GPIOC
#define FAN_Speed_2_Pin GPIO_PIN_15
#define FAN_Speed_2_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOF
#define OVERLOAD_I_Pin GPIO_PIN_1
#define OVERLOAD_I_GPIO_Port GPIOF
#define ADC_BAT_V_Pin GPIO_PIN_0
#define ADC_BAT_V_GPIO_Port GPIOA
#define ADC_BAT_GND_Pin GPIO_PIN_1
#define ADC_BAT_GND_GPIO_Port GPIOA
#define ADC_BAT_Temp_Pin GPIO_PIN_2
#define ADC_BAT_Temp_GPIO_Port GPIOA
#define ADC_U_IN_Pin GPIO_PIN_3
#define ADC_U_IN_GPIO_Port GPIOA
#define ADC_30V_Pin GPIO_PIN_6
#define ADC_30V_GPIO_Port GPIOA
#define ADC_BAT_LOAD_Pin GPIO_PIN_7
#define ADC_BAT_LOAD_GPIO_Port GPIOA
#define ADC_U_OUT_Pin GPIO_PIN_1
#define ADC_U_OUT_GPIO_Port GPIOB
#define VREF_R_Pin GPIO_PIN_2
#define VREF_R_GPIO_Port GPIOB
#define TIM1_CH1N_24VL_Pin GPIO_PIN_13
#define TIM1_CH1N_24VL_GPIO_Port GPIOB
#define BYPASS_ENABLE_Pin GPIO_PIN_15
#define BYPASS_ENABLE_GPIO_Port GPIOB
#define TIM1_CH1_24VH_Pin GPIO_PIN_8
#define TIM1_CH1_24VH_GPIO_Port GPIOA
#define PWR_ENABLE_I_Pin GPIO_PIN_12
#define PWR_ENABLE_I_GPIO_Port GPIOA
//#define LED_B_Pin GPIO_PIN_3
//#define LED_B_GPIO_Port GPIOB
//#define LED_A_Pin GPIO_PIN_4
//#define LED_A_GPIO_Port GPIOB
#define VREF_OK_Pin GPIO_PIN_5
#define VREF_OK_GPIO_Port GPIOB
#define TIM16_CH1N_Q4L_Pin GPIO_PIN_6
#define TIM16_CH1N_Q4L_GPIO_Port GPIOB
#define TIM17_CH1N_Q2L_Pin GPIO_PIN_7
#define TIM17_CH1N_Q2L_GPIO_Port GPIOB
#define TIM16_CH1_Q3H_Pin GPIO_PIN_8
#define TIM16_CH1_Q3H_GPIO_Port GPIOB
#define TIM17_CH1_Q1H_Pin GPIO_PIN_9
#define TIM17_CH1_Q1H_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
