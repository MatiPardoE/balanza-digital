/*
 * hx711.c
 *
 *  Created on: Sep 9, 2021
 *      Author: matip
 *      Based on github.com/getsiddd/HX711
 */

#include "hx711.h"

uint32_t offset;
double scale_g;
extern TIM_HandleTypeDef htim1;


//TEST
uint8_t i = 0;
uint32_t read_ticks;


void HX711_calib_harcodeado(void){
	scale_g=0.00074214855;
}

double HX711_calib(uint16_t calib_weight){
	uint8_t i = 0;
	int32_t value = 0;

	while(i<10){
		value = HX711_read_average_value(10);

		if(value != UNVALID)i++;
	}

	scale_g = calib_weight / (value * 1.0);

	return scale_g;
}

void HX711_set_scale_g(double value_scale_g){
	scale_g = value_scale_g;
}

void HX711_tare(uint8_t prom){
	uint8_t i = 0;
	int32_t value = 0;

	PD_SCK_SET_LOW;
	HAL_Delay(1);


	while(i<prom){

		value = HX711_read_average_raw(prom);

		if(value != UNVALID)i++;

	}

	offset = value;

}

uint32_t HX711_get_offset(void){
	return offset;
}

void delay_us (uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}

uint8_t HX711_is_ready(void)
{
	if(HAL_GPIO_ReadPin(PD_DT_PORT,PD_DT_PIN)==GPIO_PIN_RESET)return 0;
	return 1;
}

int32_t HX711_read_raw(void){
	static enum state_read read_state = WAIT_RDY;
	static int32_t value = 0;
	int32_t return_value = 0;
	//static uint8_t i = 0;
	//static uint32_t read_ticks;
	uint8_t ret_flag = 0;

	switch(read_state){
	case WAIT_RDY:
		if(!HX711_is_ready()){
			read_state = SET_PD_SCK_LOW;
			SET_TIMER_TO_0;
		}
		break;
	case SET_PD_SCK_LOW:
		if(GET_TIMER>10){
			PD_SCK_SET_LOW;
			read_state = SET_PD_SCK_HIGH;
			SET_TIMER_TO_0;
		}
		break;
	case SET_PD_SCK_HIGH:
		if(GET_TIMER>10){
			PD_SCK_SET_HIGH;
			read_state = READ_CELL;
			SET_TIMER_TO_0;
		}
		break;
	case READ_CELL:
		if(GET_TIMER>10){
	        value = value << 1;  //Shift MSB to the left
	        PD_SCK_SET_LOW;
	        if (DOUT_READ) {
				value += 1;
			}
	        i++;
			if (i > 23) { // ya lei todos los datos
				i=0;
				read_state = HX711_END_HIGH;
				SET_TIMER_TO_0;
			} else {
				read_state = SET_PD_SCK_HIGH;
				SET_TIMER_TO_0;
			}
		}
		break;
	case HX711_END_HIGH:
		if(GET_TIMER>10){
			PD_SCK_SET_HIGH;
			read_state = HX711_END_LOW;
			SET_TIMER_TO_0;
		}
		break;
	case HX711_END_LOW:
		if(GET_TIMER>10){
			PD_SCK_SET_LOW;
			read_state = WAIT_RDY;
		    if(value & 0x800000)
		    	value |= 0xff000000;  //Si es negativo lo retorno con signo de 32 bits
		    ret_flag = 1;
		}
		break;
	default:
		ret_flag = 0;
		read_state = WAIT_RDY;
	}

	if(ret_flag){
		return_value = value;
		value = 0;
		return return_value;
	}
	else{
		return UNVALID;
	}

}

int32_t HX711_read_average_raw(uint8_t prom){
	int32_t current_value;
	int32_t sum = 0;
	static int32_t vector_value[SAMPLE_MAX]; //variables static arrancan en 0
	static uint8_t sample = 0;

	current_value = HX711_read_raw();

	if(current_value != UNVALID){
		vector_value[sample] = current_value;

		sample++;
		sample %= prom;

		for(uint8_t j = 0 ; j < prom ; j++ ){
			sum += vector_value[j];
		}
		return sum/prom;
	} else {
		return UNVALID;
	}

}

int32_t HX711_read_average_value(uint8_t prom){
	int32_t avg = HX711_read_average_raw(prom);

	if(avg != UNVALID)
		return avg-offset;
	else
		return UNVALID;
}

int32_t HX711_read_g(){
	double value_kalman = HX711_read_average_value(3);
	int32_t weight_g;

	if (value_kalman != UNVALID) {
		weight_g = value_kalman * scale_g;
		return weight_g;
	} else {
		return UNVALID_WEIGHT;
	}

}


