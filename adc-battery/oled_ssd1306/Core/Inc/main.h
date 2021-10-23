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


/**
 * \fn 		: int count_digits(int number)
 * \brief 	: Cuenta la cantidad de digitos de un numero int
 * \details : Utilizada como funcion util para otras primitivas
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int number
 * \return 	: int digits
 * */
int count_digits(int);


/**
 * \fn 		: void printoled_battery(int state)
 * \brief 	: Imprime el icono de bateria correspondiente
 * \details : Los estados van de 4 (llena) a 0(vacia)
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int state
 * \return 	: none
 * */
void printoled_battery(int);

/**
 * \fn 		: void printoled_menu(void)
 * \brief 	: Imprime el menu principal
 * \details : none
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: none
 * \return 	: none
 * */
void printoled_menu(void);


/**
 * \fn 		: void printoled_weight(int weight,int unit)
 * \brief 	: Imprime el peso incluyendo g o kg
 * \details : El peso se ingresa en gramos, unit=0 -> g, unit=1 (peso>1000) -> kg
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int weight
 * \param 	: [in] int unit
 * \return 	: none
 * */
void printoled_weight(int,int);



/**
 * \fn 		: void printoled_number(int number)
 * \brief 	: Imprime el digito (0 a 9) recibido agregandolo al anterior
 * \details : Si recibe -1 limpia la pantalla, sino agrega un digito al numero, hasta 5
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int number
 * \return 	: none
 * */
void printoled_number(int);



/**
 * \fn 		: void printoled_price(int price)
 * \brief 	: Imprime el precio
 * \details : Incluye el simbolo $
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int price
 * \return 	: none
 * */
void printoled_price(int);





/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
