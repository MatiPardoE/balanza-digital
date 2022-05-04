/*
 * register.c
 *
 *  Created on: Nov 3, 2021
 *      Author: matip
 */

#include "register.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"

extern RTC_HandleTypeDef hrtc;


void guardarCalibracion(double input){
	uint32_t parteBaja1, parteBaja2, parteBaja3, parteAlta1, parteAlta2, parteAlta3;
	uint64_t registro1;

	registro1 = input * MULTIPLIER;

	parteBaja1 = (uint32_t) (registro1 & 0x0000000000000FFF);
	parteBaja2 = (uint32_t) ((registro1 & 0x0000000000FFF000) >> 12);
	parteBaja3 = (uint32_t) ((registro1 & 0x0000000FFF000000) >> 24);
	parteAlta1 = (uint32_t) ((registro1 & 0x0000FFF000000000) >> 36);
	parteAlta2 = (uint32_t) ((registro1 & 0x0FFF000000000000) >> 48);
	parteAlta3 = (uint32_t) ((registro1 & 0xF000000000000000) >> 60);

	HAL_RTCEx_BKUPWrite(&hrtc, 1, parteBaja1);
	HAL_RTCEx_BKUPWrite(&hrtc, 2, parteBaja2);
	HAL_RTCEx_BKUPWrite(&hrtc, 3, parteBaja3);
	HAL_RTCEx_BKUPWrite(&hrtc, 4, parteAlta1);
	HAL_RTCEx_BKUPWrite(&hrtc, 5, parteAlta2);
	HAL_RTCEx_BKUPWrite(&hrtc, 6, parteAlta3);
}

double obtenerCalibracion(){
	uint64_t parteBaja1, parteBaja2, parteBaja3, parteAlta1, parteAlta2, parteAlta3;
	uint64_t registro1;

	parteBaja1 = HAL_RTCEx_BKUPRead(&hrtc, 1);
	parteBaja2 = HAL_RTCEx_BKUPRead(&hrtc, 2);
	parteBaja3 = HAL_RTCEx_BKUPRead(&hrtc, 3);
	parteAlta1 = HAL_RTCEx_BKUPRead(&hrtc, 4);
	parteAlta2 = HAL_RTCEx_BKUPRead(&hrtc, 5);
	parteAlta3 = HAL_RTCEx_BKUPRead(&hrtc, 6);

	registro1 = parteBaja1 + (parteBaja2 << 12) + (parteBaja3 << 24) + (parteAlta1 << 36) + (parteAlta2 << 48) + (parteAlta3 << 60);

	return (double) registro1/MULTIPLIER;
}
