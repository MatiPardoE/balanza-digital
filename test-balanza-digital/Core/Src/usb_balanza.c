/*
 * usb_balanza.c
 *
 *  Created on: 24 oct. 2021
 *      Author: hernan
 */

#include <stdint.h>
#include "usb_balanza.h"

/**
 * \fn 		: void usb_balanza_init()
 * \brief 	: Asigna la funcion que administra la recepcion por puerto serie
 * \details : Asigna el puntero que recibe los datos serie
 * \author 	: Hernan Rodriguez
 * \date   	: 24/10/2021
 * \param 	: void
 * \return 	: void
 * */
void usb_balanza_init(){
	hook_rx = serial_rx;
}

/**
 * \fn 		: void handle_serial(uint8_t b)
 * \brief 	: Maquina de estados que administra la recepcion de datos por puerto serie
 * \details : Analiza byte por byte los datos que se reciben, haciendo la validacion de los mismos
 * \author 	: Hernan Rodriguez
 * \date   	: 24/10/2021
 * \param 	: [in] uint8_t b
 * \return 	: void
 * */
void handle_serial(uint8_t b){
	static uint8_t state = INIT_CMD;
	static uint8_t prev_byte;
	switch(state){
	case INIT_CMD:
		if(b == '<')
			state = RX_CMD;
		break;
	case RX_CMD:
		if(b == 'T' || b == 'C' || b == 'P' || b == 'D'){
			prev_byte = b;
			state = END_CMD;
		} else if(b >= '0' && b <= '9'){
			flag_weight_qt = 0;
			weight_qt = 0;
			weight_qt += (b-'0')*1000;
			state = RX_CEN;
		} else
			state = INIT_CMD;
		break;
	case RX_CEN:
		if(b >= '0' && b <= '9'){
			weight_qt += (b-'0')*100;
			state = RX_DEC;
		} else
			state = INIT_CMD;
		break;
	case RX_DEC:
		if(b >= '0' && b <= '9'){
			weight_qt += (b-'0')*10;
			state = RX_UNI;
		} else
			state = INIT_CMD;
		break;
	case RX_UNI:
		if(b >= '0' && b <= '9'){
			prev_byte = b;
			weight_qt += b-'0';
			state = END_CMD;
		} else
			state = INIT_CMD;
		break;
	case END_CMD:
		if(b == '>'){
			if(prev_byte == 'T'){ // tarar
				// INDICAR QUE VOY A TARAJE
				flag_pc_tare = 1;
			} else if(prev_byte == 'C'){ // calibrar
				// INDICAR QUE VOY A CALIBRACIÓN
				flag_pc_calib = 1;
			} else if(prev_byte == 'D'){ // desconectar
				// INDICAR QUE ME VOY A DESCONECTAR
				flag_pc_disconected = 1;
			} else if(prev_byte >= '0' && prev_byte <= '9'){ // valor peso
				// INDICAR QUE ME LLEGO UN PESO -> ES PARA LA CALIBRACION
				flag_weight_qt = 1;
			}else if(prev_byte == 'L'){
				flag_pc_ready=1;
			}
		}

		state = INIT_CMD;
		break;
	}
}

/**
 * \fn 		: uint16_t serial_rx(uint8_t *buf, uint16_t len)
 * \brief 	: Funcion que barre el buffer de recepción
 * \details : Itera a través del buffer de recepción y llama a la función que se encarga de analizar los datos
 * \author 	: Hernan Rodriguez
 * \date   	: 24/10/2021
 * \param 	: [in] uint8_t* buf, uint16_t len
 * \return 	: uint16_t len
 * */
uint16_t serial_rx(uint8_t *buf, uint16_t len){
	for(uint16_t i=0; i<len; i++)
		handle_serial(buf[i]);
	return len;
}

/**
 * \fn 		: void serial_tx_num(uint16_t num)
 * \brief 	: Funcion que envía por puerto serie un número
 * \details : A partir del número recibido por parámetro, genera el comando y lo envía por puerto serie
 * \author 	: Hernan Rodriguez
 * \date   	: 24/10/2021
 * \param 	: [in] uint16_t num
 * \return 	: void
 * */
void serial_tx_num(int16_t num){
	char buf[8];
	sprintf(buf, "<%04d>", num);
	CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}

/**
 * \fn 		: void serial_tx_cmd(const char* cmd)
 * \brief 	: Funcion que envía por puerto serie un comando
 * \details : A partir del caracter recibido por parámetro, genera el comando y lo envía por puerto serie
 * \author 	: Hernan Rodriguez
 * \date   	: 24/10/2021
 * \param 	: [in] const char* cmd
 * \return 	: void
 * */
void serial_tx_cmd(const char* cmd){
	char buf[4];
	sprintf(buf, "<%s>", cmd);
	CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}
