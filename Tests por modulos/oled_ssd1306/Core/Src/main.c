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
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"
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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// FUNCION UTIL PARA CONTAR LA CANTIDAD DE DIGITOS DE UN NUMERO
int count_digits(int number)
{
	int digits = 0;
	while(number)
	{
		number /= 10;
		digits++;
	}
	return digits;
}

// IMPRIME EL ICONO DE BATERIA (VALORES DE 0 A 4, 4=LLENA, 0=VACIA)
void printoled_battery(int state)
{
	SSD1306_DrawBitmap(95, 0, battery_icon_4, 32, 18, 0); //LIMPIO LA ZONA
	SSD1306_UpdateScreen();
	switch(state){
	case 0:
		SSD1306_DrawBitmap(95, 0, battery_icon_0, 32, 18, 1); //ORIGINAL
		break;
	case 1:
		SSD1306_DrawBitmap(95, 0, battery_icon_1, 32, 18, 1); //ORIGINAL
		break;
	case 2:
		SSD1306_DrawBitmap(95, 0, battery_icon_2, 32, 18, 1); //ORIGINAL
		break;
	case 3:
		SSD1306_DrawBitmap(95, 0, battery_icon_3, 32, 18, 1); //ORIGINAL
		break;
	case 4:
		SSD1306_DrawBitmap(95, 0, battery_icon_4, 32, 18, 1); //ORIGINAL
		break;
	default:
		state = 0;
	}
	SSD1306_UpdateScreen();
}

// IMPRIME EL MENU
void printoled_menu()
{
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("MENU", &Font_7x10, 1);
	SSD1306_GotoXY(0,10);
	SSD1306_Puts("1- Pesar", &Font_7x10, 1);
	SSD1306_GotoXY(0,20);
	SSD1306_Puts("2- Calibrar", &Font_7x10, 1);
	SSD1306_GotoXY(0,30);
	SSD1306_Puts("3- Tarar", &Font_7x10, 1);
	SSD1306_GotoXY(0,40);
	SSD1306_Puts("4- Calcular Precio", &Font_7x10, 1);
	SSD1306_GotoXY(0,50);
	SSD1306_Puts("5- Conectar a PC", &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

//RECIBE UN VALOR INT DE PESO(EN GRAMOS) Y LO IMPRIME EN PANTALLA INCLUYENDO 'g' SI UNIT=0 o 'kg' SI UNIT=1 Y WEIGHT ES MAYOR A 1000
//EN CASO DE UNIT=1 SOLO IMRPIME NUMEROS DE HASTA 9,999 KG (PARA ESTA BALANZA ES SUFICIENTE)
void printoled_weight(int weight,int unit)
{
	int i,w,digits;
	int div = 1;
	digits = count_digits(weight);
	if(weight < 1000)
		unit = 0;
	char weight_shown[digits+1+unit];

	if(digits > 1){
		for(i=1 ; i<digits ; i++)
			div*=10;
	}
	for(i=0 ; i<digits+unit ; i++){
		if( unit==1 && i==1 )
			weight_shown[i] = ',';
		else{
		w = (weight/div);
		weight_shown[i] = w + '0';
		weight -= w*div;
		div /= 10;
		}
	}
	weight_shown[i] = '\0';

	SSD1306_GotoXY(60-8*digits-20*unit,20);
	SSD1306_Puts(weight_shown, &Font_16x26, 1);
	if(unit){
		SSD1306_GotoXY(95,20);
		SSD1306_Puts("Kg", &Font_16x26, 1);
	}
	else{
		SSD1306_GotoXY(65+8*digits,20);
		SSD1306_Puts("g", &Font_16x26, 1);
	}
	SSD1306_UpdateScreen();
}

// IMPRIMIR EL TEXTO DE INICIO DE LA BALANZA
void printoled_start()
{
	 SSD1306_GotoXY(10,10);
	 SSD1306_Puts(" BALANZA",&Font_11x18,1);
	 SSD1306_GotoXY(10,30);
	 SSD1306_Puts(" DIGITAL", &Font_11x18, 1);
	 SSD1306_UpdateScreen();
}

//RECIBE UN VALOR ENTRE 0 Y 9 Y LO IMPRIME, AGREGANDO EL VALOR RECIBIDO AL ANTERIOR PARA FORMAR NUMEROS DE VARIOS DIGITOS (HASTA 5) A PARTIR DE CADA DIGITO
//SI RECIBE -1 ELIMINA TODA LA COLA DE NUMEROS Y BORRA LA PANTALLA
void printoled_number(int number)
{
	static int i=0;
	static char number_shown[6];
	number_shown[i] = number + '0';
	number_shown[i+1] = '\0';
	if(number >= 0){
		SSD1306_GotoXY(55-6*i,20);
		SSD1306_Puts(number_shown, &Font_16x26, 1);
		SSD1306_UpdateScreen();
		i++;
	}
	else{
		i=0;
		number_shown[0] = '\0';;
		SSD1306_Clear();
	}
}

//RECIBE UN VALOR DE PRECIO E IMPIRME INCLUYENDO EL $
void printoled_price(int price)
{
	int i,p,digits;
	int div = 1;
	digits = count_digits(price);
	char price_shown[digits+1];

	if(digits > 1){
			for(i=1 ; i<digits ; i++)
				div*=10;
	}
	/* ESTA VERSION ES CON EL SIGNO PESOS MAS CHICO PERO SE VE MEJOR*/
	SSD1306_GotoXY(52-8*digits,23);
	SSD1306_Putc('$', &Font_11x18, 1);
	for(i=0 ; i<digits ; i++)
	{
			p = (price/div);
			price_shown[i] = p + '0';
			price -= p*div;
			div /= 10;
	}
	price_shown[i] = '\0';

	SSD1306_GotoXY(65-8*digits,20);
	SSD1306_Puts(price_shown, &Font_16x26, 1);
	/**/
	/*//ESTA VERSION ES CON EL SIGNO PESOS GRANDE PERO SE VE FEO
	price_shown[0] = '$';
	for(i=1 ; i<=digits ; i++)
	{
			p = (price/div);
			price_shown[i] = p + '0';
			price -= p*div;
			div /= 10;
	}
	price_shown[i] = '\0';

	SSD1306_GotoXY(60-8*digits,20);
	SSD1306_Puts(price_shown, &Font_16x26, 1);
	*/

	SSD1306_UpdateScreen();
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  SSD1306_Init(); //INICIALIZACION



/*  SSD1306_ScrollRight(0x00, 0x0F);
  HAL_Delay(2000);
  SSD1306_ScrollLeft(0x00, 0x0F);
  HAL_Delay(2000);
  SSD1306_Stopscroll();*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
	  	printoled_start();
	  	HAL_Delay(2000);
		SSD1306_Clear();

	  	printoled_menu();
	  	printoled_battery(4);
	  	HAL_Delay(2000);
	  	printoled_battery(3);
	   	HAL_Delay(2000);
	  	printoled_battery(2);
	   	HAL_Delay(2000);
	  	printoled_battery(1);
	   	HAL_Delay(2000);
	  	printoled_battery(0);
	   	HAL_Delay(2000);
	   	SSD1306_Clear();

	    printoled_weight(3,0);
	    HAL_Delay(2000);
	    SSD1306_Clear();

	    printoled_weight(56,0);
	    HAL_Delay(2000);
	    SSD1306_Clear();

	    printoled_weight(892,1);
	    HAL_Delay(2000);
	    SSD1306_Clear();

	    printoled_weight(2394,0);
	    HAL_Delay(2000);
	    SSD1306_Clear();

	    printoled_weight(9999,1);
	    HAL_Delay(2000);
	    SSD1306_Clear();

	    //recibo e imprimo acumulando
	    printoled_number(1);
	    HAL_Delay(1000);
	    printoled_number(5);
	    HAL_Delay(1000);
	    printoled_number(0);
	    HAL_Delay(2000);
	    printoled_number(-1); //para terminar la tira de valores

	    printoled_price(2);
	    HAL_Delay(2000);
	    SSD1306_Clear();
	    printoled_price(32);
	    HAL_Delay(2000);
	    SSD1306_Clear();
	    printoled_price(470);
	    HAL_Delay(2000);
	    SSD1306_Clear();
	    printoled_price(8202);
	    HAL_Delay(2000);
	    SSD1306_Clear();



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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
