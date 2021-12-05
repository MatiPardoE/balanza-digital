/*
 * hx711.h
 *
 *  Created on: Sep 9, 2021
 *      Author: matip
 *      Based on github.com/getsiddd/HX711
 */

#ifndef INC_HX711_H_
#define INC_HX711_H_

#include "stm32f1xx_hal.h"
#include <math.h>

// These ports are configured in the init function HX711_init()
// Remember to Enable GPIO Ports Clock
#define PD_SCK_PORT         GPIOB	                        // Serial Clock Input Port
#define PD_SCK_PIN          GPIO_PIN_10                     // Serial Clock Pin
#define PD_DT_PORT         	GPIOB	                        // Data OUT Port
#define PD_DT_PIN          	GPIO_PIN_11                     // Data OUT PIN

#define PD_SCK_SET_LOW		HAL_GPIO_WritePin(PD_SCK_PORT, PD_SCK_PIN , GPIO_PIN_RESET)	//Instruction for CLK
#define PD_SCK_SET_HIGH		HAL_GPIO_WritePin(PD_SCK_PORT, PD_SCK_PIN , GPIO_PIN_SET)	//Instruction for CLK

#define DOUT_READ			HAL_GPIO_ReadPin(PD_DT_PORT, PD_DT_PIN)
#define DOUT_SET_HIGH		HAL_GPIO_WritePin(PD_DT_PORT, PD_DT_PIN,GPIO_PIN_SET)

#define SAMPLE_MAX			256 							// Maximos samples para promediar

#define DELAY_1US			72
#define UNVALID				2000000000
#define UNVALID_WEIGHT		-5000

#define SET_TIMER_TO_0		__HAL_TIM_SET_COUNTER(&htim1,0)
#define GET_TIMER			__HAL_TIM_GET_COUNTER(&htim1)

enum state_read {WAIT_RDY, SET_PD_SCK_LOW, SET_PD_SCK_HIGH, READ_CELL, HX711_END_HIGH, HX711_END_LOW};


	 void delay_us (uint32_t us);

	// check if HX711 is ready
	// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
	// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
	uint8_t HX711_is_ready(void);

	uint32_t HX711_get_offset(void);

	void HX711_set_scale_g(double value_scale_g);

	// waits for the chip to be ready and returns a reading
	int32_t HX711_read_raw(void);

	int32_t HX711_read_average_raw(uint8_t prom);

	void HX711_tare(uint8_t prom);

	int32_t HX711_read_average_value(uint8_t prom);

	int32_t HX711_read_g();

	double HX711_calib(uint16_t calib_weight);

	void HX711_calib_harcodeado(void);

#endif /* INC_HX711_H_ */
