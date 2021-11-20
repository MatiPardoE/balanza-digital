/*
 * adc_filter.h
 *
 *  Created on: Sep 26, 2021
 *      Author: tobias
 */

#ifndef INC_ADC_FILTER_H_
#define INC_ADC_FILTER_H_

#define bat_len 			10
//#define TICKS_ADC_MS		(10000)	//Tomo muestras cada 10_000 mS
#define TICKS_ADC_MS		(1000)	//Tomo muestras cada 5_000 mS
#define DELAY_ADC			(64)
#define demora_software(X)	{for(int i=0; i<(X); i++){}}

void Inicializar_avg_movil(uint32_t *buffer, uint8_t *ix, uint8_t len, uint32_t *acc);
uint16_t Set_avg_movil(uint16_t sample, uint32_t *buffer, uint8_t *ix, uint8_t len, uint32_t *acc);
//uint8_t Get_porcentage_bat(uint16_t *now_cuentas);
uint8_t Check_porcentage(uint16_t *now_cuentas);
void Measure_battery(void) ;
#endif /* INC_ADC_FILTER_H_ */
