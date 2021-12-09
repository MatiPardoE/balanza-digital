/*
 * hx711.c
 *
 *  Created on: Sep 9, 2021
 *      Author: matip
 */

#include "hx711.h"

uint32_t offset;					//Variable interna que determina el cruce por 0 de la recta de peso
double scale_g;						//Variable interna que determina la pendiente de la recta de peso
extern TIM_HandleTypeDef htim1; 	//Handler de un timer para hacer las demoras


/**
 * \fn 		: double HX711_calib(uint16_t calib_weight)
 * \brief 	: Calcula con un peso calibrado la variable que se usara en la recta de peso
 * \details : Se toman 10 muestras de manera bloqueante y lo guarda en la variable interna
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: [in] uint16_t calib_weight : Valor del Peso calibrado en gramos
 * \return 	: double : Retorna el valor guardado
 * */
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

/**
 * \fn 		: void HX711_set_scale_g(double value_scale_g)
 * \brief 	: Setea la variable interna con el valor ingresado como param
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: [in] double value_scale_g : Valor previamente calculado para la pendiente
 * \return 	: void
 * */
void HX711_set_scale_g(double value_scale_g){
	scale_g = value_scale_g;
}

/**
 * \fn 		: void HX711_tare(uint8_t prom)
 * \brief 	: Calcula la variable interna offset
 * \details : Muestrea la cantidad de veces indicadas por el param de manera bloqueante
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: [in] uint8_t prom : Cantidad de veces a muestrear y promediar
 * \return 	: void
 * */
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

/**
 * \fn 		: void HX711_tare(uint8_t prom)
 * \brief 	: Retorna la variable interna a traves de funcion
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: void
 * \return 	: uint32_t : retorna la variable interna offset
 * */
uint32_t HX711_get_offset(void){
	return offset;
}

/**
 * \fn 		: uint8_t HX711_is_ready(void)
 * \brief 	: Lee el pin para saber si el Hx711 esta listo para una nueva conversion
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: void
 * \return 	: uint8_t : retorna 1 cuando esta listo ; retorna 0 cuando esta ocupado
 * */
uint8_t HX711_is_ready(void)
{
	if(HAL_GPIO_ReadPin(PD_DT_PORT,PD_DT_PIN)==GPIO_PIN_RESET)return 1;
	return 0;
}

/**
 * \fn 		: int32_t HX711_read_raw(void)
 * \brief 	: Desarrollo del protocolo de comunicacion del Hx711 de manera serie con tiempos de 1us (Ver datasheet para facilitar comprension)
 * \details : Se ejecuta de manera NO bloqueante, retorna invalido hasta tener un dato completo.
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: void
 * \return 	: int32_t : Valor del ADC crudo
 * */
int32_t HX711_read_raw(void){
	static enum state_read read_state = WAIT_RDY;
	static int32_t value = 0;
	int32_t return_value = 0;
	static uint8_t i = 0;
	uint8_t ret_flag = 0;

	switch(read_state){
	case WAIT_RDY:
		if(HX711_is_ready()){
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

/**
 * \fn 		: int32_t HX711_read_average_raw(uint8_t prom)
 * \brief 	: Promedia las mediciones del ADC
 * \details : Se ejecuta de manera NO bloqueante a traves de un vector tipo Pipeline
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: [in] prom : Cantidad de muestras a promediar
 * \return 	: int32_t : Valor del ADC crudo promediado
 * */
int32_t HX711_read_average_raw(uint8_t prom){
	int32_t current_value;
	int32_t sum = 0;
	static int32_t vector_value[SAMPLE_MAX];
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

/**
 * \fn 		: int32_t HX711_read_average_value(uint8_t prom)
 * \brief 	: Promedia las mediciones del ADC y le resta el offset
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: [in] prom : Cantidad de muestras a promediar
 * \return 	: int32_t : Valor del ADC promediado con la resta del offset
 * */
int32_t HX711_read_average_value(uint8_t prom){
	int32_t avg = HX711_read_average_raw(prom);

	if(avg != UNVALID)
		return avg-offset;
	else
		return UNVALID;
}

/**
 * \fn 		: int32_t HX711_read_g()
 * \brief 	: Convierte el valor del ADC con el offset a gramos y redondea
 * details  : Se opto un muestreo de 3
 * \author 	: Matias Pardo E.
 * \date   	: 14/09/2021
 * \param 	: void
 * \return 	: int32_t : Valor en Gramos con offset incluido
 * */
int32_t HX711_read_g(){
	double value = HX711_read_average_value(3);
	int32_t weight_g;

	if (value != UNVALID) {
		weight_g = round(value * scale_g);
		return weight_g;
	} else {
		return UNVALID_WEIGHT;
	}

}


