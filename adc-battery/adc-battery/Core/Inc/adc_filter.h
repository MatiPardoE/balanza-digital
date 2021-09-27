/*
 * adc_filter.h
 *
 *  Created on: Sep 26, 2021
 *      Author: tobias
 */

#ifndef INC_ADC_FILTER_H_
#define INC_ADC_FILTER_H_

#define bat_len 			10

void Inicializar_avg_movil(uint32_t *buffer, uint8_t *ix, uint8_t len, uint32_t *acc);
int Set_avg_movil(uint16_t sample, uint32_t *buffer, uint8_t *ix, uint8_t len, uint32_t *acc);
uint8_t Get_porcentage_bat(uint16_t *now_cuentas);
uint8_t Check_porcentage(uint16_t *now_cuentas);
#endif /* INC_ADC_FILTER_H_ */
