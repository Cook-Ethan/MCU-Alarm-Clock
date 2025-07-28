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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define Alarm_Indicator_Pin GPIO_PIN_0
#define Alarm_Indicator_GPIO_Port GPIOC
#define Pm_Indicator_Pin GPIO_PIN_1
#define Pm_Indicator_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define Set_Clock_Pin GPIO_PIN_6
#define Set_Clock_GPIO_Port GPIOA
#define Set_Alarm_Pin GPIO_PIN_7
#define Set_Alarm_GPIO_Port GPIOA
#define Snooze_Pin GPIO_PIN_8
#define Snooze_GPIO_Port GPIOA
#define Snooze_EXTI_IRQn EXTI9_5_IRQn
#define Hour_Increment_Pin GPIO_PIN_9
#define Hour_Increment_GPIO_Port GPIOA
#define Hour_Increment_EXTI_IRQn EXTI9_5_IRQn
#define Minute_Increment_Pin GPIO_PIN_10
#define Minute_Increment_GPIO_Port GPIOA
#define Minute_Increment_EXTI_IRQn EXTI15_10_IRQn
#define Alarm_On_Pin GPIO_PIN_11
#define Alarm_On_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
