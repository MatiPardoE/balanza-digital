/*
 * hx711.c
 *
 *  Created on: Sep 9, 2021
 *      Author: matip
 *      Based on github.com/getsiddd/HX711
 */

#include "hx711.h"

uint32_t offset;
float scale_g;

double KALMAN(int32_t U){
	  static const double R = 10;
	  static const double H = 1.00; // Especie de controlador de KALMAN
	  static double Q=10;
	  static double P=15;
	  static double U_hat=0;
	  static double Kg=0;

	  Kg=P*H/(H*P*H+R);
	  U_hat=U_hat+Kg*(U-H*U_hat);

	  P=(1-Kg*H)*P+Q;

	  return U_hat;
}

void HX711_calib_harcodeado(void){
	scale_g=0.00074912;
}

float HX711_calib(uint32_t calib_weight){

	scale_g = calib_weight / HX711_read_average_value(10);

	return scale_g;
}

void HX711_set_scale_g(float value_scale_g){
	scale_g = value_scale_g;
}

void HX711_tare(void){
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

int32_t HX711_read_g(){
	uint8_t i=0;
	int32_t value_unscaled,current_weight_g;
	double value_kalman,sum_value_kalman=0;

	for(i=0;i<4;i++){
		value_unscaled = HX711_read_average_value(3);
		value_kalman = KALMAN(value_unscaled);
		sum_value_kalman += value_kalman;
	}
	current_weight_g = (sum_value_kalman/4)*scale_g;//Divido por 4 para el prom

	return current_weight_g;
}

extern TIM_HandleTypeDef htim1;

void delay_us (uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}
