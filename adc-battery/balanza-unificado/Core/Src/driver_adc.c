/*
 * driver_adc.h
 *
 *  Created on: Oct 23, 2021
 *      Author: tobias
 */

#include "main.h"

extern uint8_t charge_por;

uint32_t ticks_adc;
//Borrar a futuro
uint16_t cuentas_adc;
//

uint8_t bat_index;
uint32_t bat_buffer[bat_len];
uint32_t bat_acc;

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
 * \return 	: resultado del promedio de la bateria
 * */

int Set_avg_movil(uint16_t sample, uint32_t *buffer, uint8_t *ix, uint8_t len,
		uint32_t *acc) {
	*acc += sample - buffer[*ix];
	buffer[*ix] = sample;
	if (++*ix == len)
		*ix = 0;
	return (*acc / len);
}

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
	//vmax = 3.9V
	//vmin = 3v
	if (*now_cuentas < 2233) // 3*150/250*4095/3.3
		return (0);
	else if (*now_cuentas < 2400)
		return (1);
	else if (*now_cuentas < 2567)
		return (2);
	else if (*now_cuentas < 2734)
		return (3);
	else
		return (4);

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
	} else
		return (charge_por);
}

