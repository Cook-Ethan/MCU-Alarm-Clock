/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const uint16_t digit_pins[4] = {
		Dig_1_Pin,
		Dig_2_Pin,
		Dig_3_Pin,
		Dig_4_Pin
};
const uint16_t segment_pins[8] = {
		Seg_A_Pin,
		Seg_B_Pin,
		Seg_C_Pin,
		Seg_D_Pin,
		Seg_E_Pin,
		Seg_F_Pin,
		Seg_G_Pin,
		Seg_H_Pin
};
const uint8_t segment_map[16] = {
		0x3F,	// 0
		0x06,	// 1
		0x5B,	// 2
		0x4F,	// 3
		0x66,	// 4
		0x6D,	// 5
		0x7D,	// 6
		0x07,	// 7
		0x7F,	// 8
		0x6F,	// 9
		0x77,	// A
		0x7C,	// b
		0x39,	// C
		0x5E,	// d
		0x79,	// E
		0x71	// F
};
char uart_buf[64];
volatile uint8_t clock_hour = 0;
volatile uint8_t clock_minute = 0;
volatile uint8_t alarm_hour = 0;
volatile uint8_t alarm_minute = 0;

volatile uint8_t current_digit = 0;

bool alarm_enabled;
bool alarm_triggered = false;

volatile bool is_display_alarm = 0;

volatile bool wakeup_time_update = true;
volatile bool wakeup_alarm_state = true;
volatile bool wakeup_hour_increment = false;
volatile bool wakeup_minute_increment = false;
volatile bool wakeup_alarm_display = false;
volatile bool wakeup_snooze = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void update_display(void);
void write_segment(uint8_t seg);
void disable_all_digits(void);
void update_time(void);
void handle_alarm_switch(void);
void handle_time_adjust(void);
void handle_snooze(void);
void update_display_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}
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
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  printf("Alarm clock started.\r\n");
  HAL_TIM_Base_Start_IT(&htim7);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if (wakeup_time_update) {
		wakeup_time_update = false;
		update_time();
	}
	if (wakeup_alarm_state) {
		wakeup_alarm_state = false;
		handle_alarm_switch();
	}
	if (wakeup_hour_increment || wakeup_minute_increment) {
		handle_time_adjust();
	}
	if (wakeup_snooze) {
		handle_snooze();
	}
	if (wakeup_alarm_display) {
		update_display_mode();
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	__WFI();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	wakeup_time_update = true;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == Set_Alarm_Pin) {
		wakeup_alarm_display = true;
	} else if (GPIO_Pin == Alarm_On_Pin) {
		wakeup_alarm_state = true;
	} else if (GPIO_Pin == Snooze_Pin) {
		wakeup_snooze = true;
	} else if (GPIO_Pin == Hour_Increment_Pin) {
		wakeup_hour_increment = true;
	} else if (GPIO_Pin == Minute_Increment_Pin) {
		wakeup_minute_increment = true;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) {
		HAL_GPIO_TogglePin(GPIOC, Buzzer_Pin);
	} else if (htim->Instance == TIM7) {
		update_display();
	}
}

void update_display()
{
	disable_all_digits();

	uint8_t hour, minute;
	bool is_pm;

	if (is_display_alarm) {
		hour = (alarm_hour % 12) ? alarm_hour % 12 : 12;
		minute = alarm_minute;
		is_pm = alarm_hour >= 12;
	} else {
		hour = (clock_hour % 12) ? clock_hour % 12 : 12;
		minute = clock_minute;
		is_pm = clock_hour >= 12;
	}

	uint8_t display[4] = {
			(hour / 10) & 0x0F,
			(hour % 10) & 0x0F,
			(minute / 10) & 0x0F,
			(minute % 10) & 0x0F
	};

	uint8_t seg = segment_map[display[current_digit]];
	if (current_digit == 0 && display[0] == 0) {
		seg = 0x00;
	}

	write_segment(seg);
	HAL_GPIO_WritePin(GPIOB, digit_pins[current_digit], GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOC, Pm_Indicator_Pin, (GPIO_PinState) is_pm);

	current_digit = (current_digit + 1) % 4;
}

void write_segment(uint8_t seg)
{
	for (int i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(GPIOB, segment_pins[i], (seg & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void disable_all_digits()
{
	for (int i = 0; i < 4; i++) {
		HAL_GPIO_WritePin(GPIOB, digit_pins[i], GPIO_PIN_SET);
	}
}

void update_time(void)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	clock_hour = time.Hours;
	clock_minute = time.Minutes;

	if (clock_hour == alarm_hour && clock_minute == alarm_minute) {
		if (alarm_enabled) {
			alarm_triggered = true;
			__HAL_TIM_SET_COUNTER(&htim6, 0);
			HAL_TIM_Base_Start_IT(&htim6);
			HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, GPIO_PIN_SET);
		}
	}
}

void handle_alarm_switch(void)
{
	if (HAL_GPIO_ReadPin(GPIOA, Alarm_On_Pin) == GPIO_PIN_RESET) {
		if (alarm_triggered) {
			HAL_TIM_Base_Stop_IT(&htim6);
			HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(GPIOC, Alarm_Indicator_Pin, GPIO_PIN_RESET);
		alarm_enabled = false;
		alarm_triggered = false;
	} else {
		HAL_GPIO_WritePin(GPIOC, Alarm_Indicator_Pin, GPIO_PIN_SET);
		alarm_enabled = true;
	}
}

void handle_time_adjust(void)
{
	if (wakeup_hour_increment) {
		wakeup_hour_increment = false;
		if (HAL_GPIO_ReadPin(GPIOA, Set_Clock_Pin) == GPIO_PIN_RESET) {
			clock_hour = (clock_hour + 1) % 24;

			RTC_SetTime(clock_hour, clock_minute, 0);
		}
		if (HAL_GPIO_ReadPin(GPIOA, Set_Alarm_Pin) == GPIO_PIN_RESET) {
			alarm_hour = (alarm_hour + 1) % 24;
		}
	}
	if (wakeup_minute_increment) {
		wakeup_minute_increment = false;
		if (HAL_GPIO_ReadPin(GPIOA, Set_Clock_Pin) == GPIO_PIN_RESET) {
			clock_minute = (clock_minute + 1) % 60;

			RTC_SetTime(clock_hour, clock_minute, 0);
		}
		if (HAL_GPIO_ReadPin(GPIOA, Set_Alarm_Pin) == GPIO_PIN_RESET) {
			alarm_minute = (alarm_minute + 1) % 60;
		}
	}
}

void handle_snooze(void)
{
	wakeup_snooze = false;
	if (alarm_triggered) {
		alarm_hour = (clock_hour + clock_minute/60) % 24;
		alarm_minute = (clock_minute + 9) % 60;
		alarm_triggered = false;

		HAL_TIM_Base_Stop_IT(&htim6);
		HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, GPIO_PIN_RESET);
	}
}

void update_display_mode(void)
{
	wakeup_alarm_display = false;
	is_display_alarm = HAL_GPIO_ReadPin(GPIOA, Set_Alarm_Pin) == GPIO_PIN_RESET;
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
