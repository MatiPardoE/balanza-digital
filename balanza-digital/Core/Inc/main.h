/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "adc_balanza.h"
#include "debounce.h"
#include "keypad_balanza.h"
//#include "bitmap.h"
#include "fonts.h"
#include "ssd1306.h"
#include "oled_balanza.h"
#include "hx711.h"

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
void Init_balanza(void);

void Compare_print(int16_t last,int16_t new, uint8_t type);
#define BAT 0
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ROW_1_Pin GPIO_PIN_1
#define ROW_1_GPIO_Port GPIOA
#define ROW_2_Pin GPIO_PIN_2
#define ROW_2_GPIO_Port GPIOA
#define ROW_3_Pin GPIO_PIN_3
#define ROW_3_GPIO_Port GPIOA
#define ROW_4_Pin GPIO_PIN_4
#define ROW_4_GPIO_Port GPIOA
#define COL_1_Pin GPIO_PIN_5
#define COL_1_GPIO_Port GPIOA
#define COL_2_Pin GPIO_PIN_6
#define COL_2_GPIO_Port GPIOA
#define COL_3_Pin GPIO_PIN_7
#define COL_3_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define WAIT_VIEW 2000	//Espero 2000 ms
#define LOW_LIMIT -3000
#define UNVALID_VALUE   -5000


enum state {WELCOME, WEIGHTHING, CALIBRATE, TARE, PRICE, PC, MENU, CALIB_KEY, PRICE_VIEW};

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
