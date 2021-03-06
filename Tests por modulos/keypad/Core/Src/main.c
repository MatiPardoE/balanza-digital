/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debounce.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_TICKS 	(4000)
#define KEYPAD_TICKS	(8000)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
debounce_t deb_col_1;
debounce_t deb_col_2;
debounce_t deb_col_3;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void set_row(uint8_t row){
	switch(row){
	case 1:
		HAL_GPIO_WritePin(GPIOA, ROW_4_Pin|ROW_3_Pin|ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, ROW_1_Pin, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, ROW_4_Pin|ROW_3_Pin|ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, ROW_2_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, ROW_4_Pin|ROW_2_Pin|ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, ROW_3_Pin, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOA, ROW_3_Pin|ROW_2_Pin|ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, ROW_4_Pin, GPIO_PIN_RESET);
		break;
	default:
		HAL_GPIO_WritePin(GPIOA, ROW_4_Pin|ROW_3_Pin|ROW_2_Pin|ROW_1_Pin, GPIO_PIN_SET);
		break;
	}
}

uint8_t read_cols(){
	debounce_check(&deb_col_1, HAL_GPIO_ReadPin(GPIOA, COL_1_Pin));
	if(debounce_edge(&deb_col_1))
		return 1;
	debounce_check(&deb_col_2, HAL_GPIO_ReadPin(GPIOA, COL_2_Pin));
	if(debounce_edge(&deb_col_2))
		return 2;
	debounce_check(&deb_col_3, HAL_GPIO_ReadPin(GPIOA, COL_3_Pin));
	if(debounce_edge(&deb_col_3))
		return 3;
	return 0;
}

uint8_t read_keypad(){
	static uint8_t current_row = 1;
	static uint32_t keypad_millis = KEYPAD_TICKS;
	uint8_t current_col = 0;

	current_col = read_cols();
	if(current_col){
		return (((current_row-1)*3) + current_col);
	}

	switch(current_row){
	case 1:
		if(!(--keypad_millis)){
			set_row(2);
			keypad_millis = KEYPAD_TICKS;
			current_row = 2;
		}
		break;
	case 2:
		if(!(--keypad_millis)){
			set_row(3);
			keypad_millis = KEYPAD_TICKS;
			current_row = 3;
		}
		break;
	case 3:
		if(!(--keypad_millis)){
			set_row(4);
			keypad_millis = KEYPAD_TICKS;
			current_row = 4;
		}
		break;
	case 4:
		if(!(--keypad_millis)){
			set_row(1);
			keypad_millis = KEYPAD_TICKS;
			current_row = 1;
		}
		break;
	default:
		set_row(1);
		keypad_millis = KEYPAD_TICKS;
		current_row = 1;
		break;
	}
	return 0;
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
  /* USER CODE BEGIN 2 */
  uint8_t key;

  debounce_init(&deb_col_1, 1, DEBOUNCE_TICKS);
  debounce_init(&deb_col_2, 1, DEBOUNCE_TICKS);
  debounce_init(&deb_col_3, 1, DEBOUNCE_TICKS);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  key = read_keypad();
	  if(key)
		  HAL_GPIO_TogglePin(BUILTIN_LED_GPIO_Port, BUILTIN_LED_Pin);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUILTIN_LED_GPIO_Port, BUILTIN_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ROW_1_Pin|ROW_2_Pin|ROW_3_Pin|ROW_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUILTIN_LED_Pin */
  GPIO_InitStruct.Pin = BUILTIN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUILTIN_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW_1_Pin ROW_2_Pin ROW_3_Pin ROW_4_Pin */
  GPIO_InitStruct.Pin = ROW_1_Pin|ROW_2_Pin|ROW_3_Pin|ROW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : COL_1_Pin COL_2_Pin COL_3_Pin */
  GPIO_InitStruct.Pin = COL_1_Pin|COL_2_Pin|COL_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
