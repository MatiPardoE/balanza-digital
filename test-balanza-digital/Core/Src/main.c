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
#include "usb_device.h"

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
#include "register.h"
#include "usb_balanza.h"
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// START VARIABLE
uint32_t tick_main;
// END VARIABLE

// START VARIABLE SRAM
float escala_sram;
uint32_t offset_sram;
// END VARIABLE SRAM

// START VARIABLE KEYPAD
extern debounce_t deb_col_1; 	//! Variable para inicializar
extern debounce_t deb_col_2;	//! Variable para inicializar
extern debounce_t deb_col_3;	//! Variable para inicializar

uint8_t key = 0;			// DEBE SER LOCAL
enum state s = WELCOME;	// DEBE SER LOCAL
int32_t new_w = 0;			// DEBE SER LOCAL
int32_t last_w = -1;		// DEBE SER LOCAL
int16_t last_bat = 255;		// DEBE SER LOCAL
int16_t charge_por; 		// DEBE SER LOCAL, sería new_bat
uint16_t calib_val = 0;		// DEBE SER LOCAL
//uint8_t new_bat = 4;		// DEBE SER LOCAL
uint8_t flag_print,flag_pc_ready,flag_pc_tare,flag_pc_calib,flag_pc_disconected,flag_weight_qt; //DEBE SER GLOBAL
uint16_t weight_qt;

// END VARIABLE KEYPAD

// START VARIABLE ADC
uint16_t bat_avg;	//! Es el promedio de muestras
int16_t charge_por; //! Variable global para la medicion de bateria

extern uint32_t ticks_adc;				//! Uso interno para hacer pooling
extern uint16_t cuentas_adc;			//! Uso interno para convertir
extern uint8_t bat_index;				//! Usada para avg movl
extern uint32_t bat_buffer[bat_len];	//! Usada para avg movl
extern uint32_t bat_acc;				//! Usada para avg movl
// END VARIABLE ADC

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	/*
	 enum state s = BIENVENIDA;

	 int32_t new_w = 0;
	 int32_t last_w = -1;
	 */

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	Init_balanza();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	ticks_adc = HAL_GetTick();					// Variable para ADC

	/* Lectura calibraciones anteriores	*/
	HX711_set_scale_g(obtenerCalibracion());

	tick_main = HAL_GetTick();

	while (1) {
		key = read_keypad();

		charge_por = Measure_battery(); 		// Toma muestras cada 5s y lo promedia
		if((charge_por != UNVALID_VALUE) && (charge_por != last_bat)){
			last_bat = charge_por;
		}

		new_w = HX711_read_g(); // pesamos todo el tiempo y no solo en WEIGHTHING
		if ((new_w > LOW_LIMIT) && (last_w != new_w)){
			flag_print = 1;
			last_w = new_w;
		}

		switch (s) {
		case WELCOME:
			printoled_start();
			HX711_tare(40);	//Tarado bloqueante
			s = WEIGHTHING;
			SSD1306_Clear();
			break;

		case MENU:
			if (key > WELCOME && key < MENU) {
				s = key;	//LA TECLA QUE SE TOCA ME MUEVE
				SSD1306_Clear();

				if (key == CALIBRATE) printoled_msg(0);

				if (key == PRICE) printoled_msg(2);

				tick_main = HAL_GetTick();

				if(key == PC) printoled_PC(0);
			}

			break;

		case WEIGHTHING:
			if (flag_print){
				SSD1306_Clear();
				printoled_weight(last_w, 0);
				flag_print = 0;
			}
			if (key == 10) {
				s = MENU;
				SSD1306_Clear();
				printoled_menu();
				printoled_battery(last_bat);
			}
			break;

		case CALIBRATE:
			if (key != 0) {
				if (key != 12) {
					if (key == 10) {
						calib_val /= 10;
					} else {
						calib_val *= 10;
						if (key == 11)
							key = 0;
						calib_val += key;
					}
					SSD1306_Clear();
					printoled_weight(calib_val, 0);
				} else {
					printoled_msg(1);
					guardarCalibracion(HX711_calib(calib_val));
					calib_val = 0;
					s = WEIGHTHING;
				}
			}

			break;
		case TARE:
			printoled_tare();
			HX711_tare(40);
			s = WEIGHTHING;
			last_w = 0; // Modificamos para que reimprima la primera vez
			break;

		case PRICE:
			if (key != 0) {
				if (key != 12) {
					if (key == 10) {
						calib_val /= 10;
					}else{
						calib_val *= 10;
						if (key == 11)
							key = 0;
						calib_val += key;
					}
					SSD1306_Clear();
					printoled_price(calib_val, 0);
				} else {
					SSD1306_Clear();
					printoled_price((calib_val*last_w)/1000.0, 1);
					calib_val = 0;
					s = PRICE_VIEW;
					tick_main = HAL_GetTick();
				}
			}
			break;

		case PRICE_VIEW:
			if (key == 10) {
				s = MENU;
				SSD1306_Clear();
				printoled_menu();
				printoled_battery(last_bat);
			}
			break;

		case PC:
			if (flag_print) {
				serial_tx_num(last_w);
			}
			if(flag_pc_tare){
				flag_pc_tare = 0;
				HX711_tare(40);
			}
			if(flag_pc_calib && flag_weight_qt){
				flag_weight_qt = 0 ;
				flag_pc_calib = 0;
				guardarCalibracion(HX711_calib(weight_qt));
			}
			if(flag_pc_ready){
				flag_pc_ready = 0;
				SSD1306_Clear();
				printoled_PC(1);
			}
			if (flag_pc_disconected) {
				flag_pc_disconected = 0;
				s = MENU;
				SSD1306_Clear();
				printoled_menu();
				printoled_battery(last_bat);
			}
			if(key == 10){
				s = MENU;
				SSD1306_Clear();
				printoled_menu();
				printoled_battery(last_bat);
			}
			break;

		default:
			break;

		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
	HAL_TIM_Base_Start(&htim1);
  /* USER CODE END TIM1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ROW_1_Pin|ROW_2_Pin|ROW_3_Pin|ROW_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
 * \fn 		: void Init_balanza(void)
 * \brief 	: Inicializa todos los modulos necesarios
 * \details : inicializa teclado.ssd,adc,filtro
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: [in] void
 * \return 	: void
 * */
void Init_balanza(void) {
	SSD1306_Init(); 								// INICIALIZACION OLED

	debounce_init(&deb_col_1, 1, DEBOUNCE_TICKS); 	//Incializo teclado
	debounce_init(&deb_col_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_col_3, 1, DEBOUNCE_TICKS);

	HAL_ADCEx_Calibration_Start(&hadc1);			// Calibro interno ADC
	Inicializar_avg_movil(bat_buffer, &bat_index, bat_len, &bat_acc);// Inicializo filtro media movil

	usb_balanza_init(); //Asigno Hook Rx
}
/*
void Compare_print(int16_t last, int16_t new, uint8_t type) {
	if (new != last) {
		last = new;
		switch(type){
		case WEIGHTHING:
			SSD1306_Clear();	//Limpio OLED
			printoled_weight(last, 0);
			break;
		case MENU:
			//SSD1306_Clear();	//Limpio OLED
			printoled_battery(last);
			break;
		case PC:
			serial_tx_num(last);
			break;
		}
	}
}
*/


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
	while (1) {
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
