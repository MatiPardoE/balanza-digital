/*
 * adc_balanza.c
 *
 *  Created on: Oct 23, 2021
 *      Author: tobias
 */

#include "main.h"

extern uint16_t bat_avg;
extern uint8_t charge_por;
extern ADC_HandleTypeDef hadc1;

uint32_t ticks_adc;				//! Uso interno para hacer pooling
uint16_t cuentas_adc;			//! Uso interno para convertir
uint8_t bat_index;				//! Usada para avg movl
uint32_t bat_buffer[bat_len];	//! Usada para avg movl
uint32_t bat_acc;				//! Usada para avg movl

/**
 * \fn 		: void Inicializar_avg_movil(int* buffer, int* ix, int len, int* acc)
 * \brief 	: Inicializa los parametros para promediar una muestra
 * \details : Trababaja con un buffer y setenado todo en '0'
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: [in] int* buffer
 * \param	: [in] int* ix
 * \param 	: [in] int len
 * \param 	: [in] int* acc
 * \return 	: void
 * */

void Inicializar_avg_movil(uint32_t *buffer, uint8_t *ix, uint8_t len,
		uint32_t *acc) {
	int i;
	*ix = 0;
	*acc = 0;
	for (i = 0; i < len; i++)
		buffer[i] = 0;
}

/**
 * \fn 		: int Set_avg_movil(int sample, int *buffer, int *ix, int len, int *acc)
 * \brief 	: Inicializa los parametros para promediar una muestra
 * \details : Actualiza el acumulado eliminando la última muestra en el buffer y devolviendo el promeedio
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: [in] int* buffer
 * \param	: [in] int* ix
 * \param 	: [in] int len
 * \param 	: [in] int* acc
 * \return 	: uint16_t : resultado del promedio de la bateria
 * */

uint16_t Set_avg_movil(uint16_t sample, uint32_t *buffer, uint8_t *ix, uint8_t len,
		uint32_t *acc) {
	*acc += sample - buffer[*ix];
	buffer[*ix] = sample;
	if (++*ix == len)
		*ix = 0;
	return (*acc / len);
}

/**
 * \fn 		: uint8_t Get_porcentage_bat(uint16_t *now_cuentas)
 * \brief 	: Calcula la carga
 * \details : Estima el valor de carga si nota una diferencia en el ADC
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: [in] int* now_cuentas
 * \return 	: valor de la carga
 * */
/*
 uint8_t Get_porcentage_bat(uint16_t *now_cuentas) {
 static uint16_t min_cuentas = 0;
 static uint16_t max_cuentas = 0;
 if (*now_cuentas - max_cuentas > 100) {
 max_cuentas = *now_cuentas;
 return (Check_porcentage(now_cuentas));
 }
 if (min_cuentas - *now_cuentas > 100) {
 min_cuentas = *now_cuentas;
 return (Check_porcentage(now_cuentas));
 }
 else
 return (charge_por);
 }
 */

/**
 * \fn 		: uint8_t Check_porcentage(uint16_t *now_cuentas)
 * \brief 	: Veriica la carga
 * \details : Lo utilizo si antes halle una diferencia grande
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: [in] int* now_cuentas
 * \return 	: valor de la carga
 * */
uint8_t Check_porcentage(uint16_t *now_cuentas) {
	if (*now_cuentas < 2110)
		return (0);
	else if (*now_cuentas < 2220)
		return (1);
	else if (*now_cuentas < 2330)
		return (2);
	else if (*now_cuentas < 2440)
		return (3);
	else
		return (4);

}

/**
 * \fn 		: void Measure_battery()
 * \brief 	: Es la capa más alta del ADC
 * \details : Controla la conversión del ADC1_IN8
 * \author 	: Tobias Bavasso Piizzi
 * \date   	: 26/09/2021
 * \param 	: void
 * \param	: void
 * \param 	: void
 * \param 	: void
 * \return 	: void
 * */

void Measure_battery(void) {
	if ((HAL_GetTick() - ticks_adc) >= TICKS_ADC_MS) {
		ticks_adc = HAL_GetTick();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		demora_software(DELAY_ADC);
		cuentas_adc = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Stop(&hadc1);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		bat_avg = Set_avg_movil(cuentas_adc, bat_buffer, &bat_index,
		bat_len, &bat_acc);
		charge_por = Check_porcentage(&bat_avg);
	}
}
