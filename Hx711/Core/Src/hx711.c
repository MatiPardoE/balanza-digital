/*
 * hx711.c
 *
 *  Created on: Sep 9, 2021
 *      Author: matip
 *      Based on github.com/getsiddd/HX711
 */

#include "hx711.h"



void HX711_calibracion(void){
	PD_SCK_SET_LOW;
	HAL_Delay(1);
	HX711_read_average_raw(10);
	offset=HX711_read_average_raw(20);
}

uint32_t HX711_get_offset(void){
	return offset;
}

uint8_t HX711_is_ready(void)
{
	if(HAL_GPIO_ReadPin(PD_DT_PORT,PD_DT_PIN)==GPIO_PIN_RESET)return 0;
	return 1;
}

int32_t HX711_read_raw(void)
{
    int32_t value=0;
    uint8_t i;
	// wait for the chip to become ready
	while (HX711_is_ready());


	delay_us(1);

    PD_SCK_SET_LOW;

    delay_us(1);

    for(i=0;i<24;i++)
    {
        PD_SCK_SET_HIGH;
        delay_us(1);

        value = value << 1;  //Shift MSB to the left

        if(DOUT_READ){
                	value+=1;
        }

        PD_SCK_SET_LOW;
        delay_us(1);


    }
    //value = value>>6;		//Divido la muestra por 2^6=64 para tener mas estabilidad

    PD_SCK_SET_HIGH;
    delay_us(1);

    if(value & 0x800000){
    	value |= 0xff000000;  //Si es negativo lo retorno con signo de 32 bits
    }

    PD_SCK_SET_LOW;

    return(value);
}

int32_t HX711_read_average_raw(uint8_t prom){
	int32_t sumatoria=0;
	uint8_t i=0;

	for(i=prom;i>0;i--){
		sumatoria += HX711_read_raw();
	}
	return (sumatoria/prom);
}

int32_t HX711_read_average_value(uint8_t prom){
	return (HX711_read_average_raw(prom)-offset);
}
