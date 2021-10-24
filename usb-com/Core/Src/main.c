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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Maquina de estados general
#define WEIGHING	0 // Estado por defecto
#define CALIBRATION	1
#define TARE		2
#define PRICES		3
#define PC			4

// Maquina de estados Serial RX
#define INIT_CMD    0
#define RX_CMD      1
#define RX_CEN      2
#define RX_DEC      3
#define RX_UNI      4
#define END_CMD     5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t general_state = WEIGHING;

uint16_t(*hook_rx)(uint8_t *b, uint16_t len);
extern USBD_HandleTypeDef hUsbDeviceFS;
uint16_t weight_qt = 0;
uint8_t flag_weight_qt = 0;
uint32_t flagtx;
uint8_t buftx[128];

uint16_t recibir(uint8_t *b, uint16_t len)
{
	int i;
	for(i=0;i<len;i++) buftx[i]=b[i];
	flagtx = len;
	return len;
}

void handle_serial(uint8_t b){
	static uint8_t state = INIT_CMD;
	static uint8_t prev_byte;
	switch(state){
	case INIT_CMD:
		if(b == '<'){
			state = RX_CMD;
		}
		break;
	case RX_CMD:
		if(b == 'T' || b == 'C' || b == 'P' || b == 'D'){
			prev_byte = b;
			state = END_CMD;
		} else if(b >= '0' && b <= '9'){
			flag_weight_qt = 0;
			weight_qt = 0;
			weight_qt += (b-'0')*1000;
			state = RX_CEN;
		} else {
			state = INIT_CMD;
		}
		break;
	case RX_CEN:
		if(b >= '0' && b <= '9'){
			weight_qt += (b-'0')*100;
			state = RX_DEC;
		} else {
			state = INIT_CMD;
		}
		break;
	case RX_DEC:
		if(b >= '0' && b <= '9'){
			weight_qt += (b-'0')*10;
			state = RX_UNI;
		} else {
			state = INIT_CMD;
		}
		break;
	case RX_UNI:
		if(b >= '0' && b <= '9'){
			prev_byte = b;
			weight_qt += b-'0';
			state = END_CMD;
		} else {
			state = INIT_CMD;
		}
		break;
	case END_CMD:
		if(b == '>'){
			if(prev_byte == 'T'){ // tarar
				general_state = TARE;
			} else if(prev_byte == 'C'){ // calibrar
				general_state = CALIBRATION;
			} else if(prev_byte == 'P'){ // pesaje
				general_state = WEIGHING; // COMO RESOLVEMOS ESTO?
			} else if(prev_byte == 'D'){ // desconectar
				general_state = WEIGHING; // COMO RESOLVEMOS ESTO?
			} else if(prev_byte >= '0' && prev_byte <= '9'){ // valor peso
				flag_weight_qt = 1;
			}
		}
		state = INIT_CMD;
		break;
	}
}

uint16_t serial_rx(uint8_t *buf, uint16_t len){
	int i;
	for(i=0;i<len;i++)
		handle_serial(buf[i]);
	return len;
}

void serial_tx_num(uint16_t num){
	char buf[8];
	sprintf(buf, "<%04d>", num);
	CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}

void serial_tx_cmd(const char* cmd){
	char buf[4];
	sprintf(buf, "<%s>", cmd);
	CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	hook_rx = recibir;

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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(flagtx)
	  {
		  //int len = flagtx;
		  flagtx=0;
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_num(3000);
		  HAL_Delay(2000);
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_num(300);
		  HAL_Delay(2000);
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_num(30);
		  HAL_Delay(2000);
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_num(3);
		  HAL_Delay(2000);
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_cmd("L");
		  HAL_Delay(2000);
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  serial_tx_cmd("D");
		  //CDC_Transmit_FS(buftx, len);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
