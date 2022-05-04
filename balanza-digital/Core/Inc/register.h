/*
 * register.h
 *
 *  Created on: Nov 3, 2021
 *      Author: matip
 */

#ifndef INC_REGISTER_H_
#define INC_REGISTER_H_

#define MULTIPLIER 10000000000


/* USER CODE BEGIN PFP */
/**
 * \fn 		: void guardarCalibracion(double registro1, double registro2);
 * \brief 	: Guarda los valores de los registros en memoria
 * \details : Recibe variable de 64 bits para guardar en 2 registros de 32
 * \author 	: Martin Rivas
 * \date   	: 23/10/2021
 * \param 	: [in] double registro1, double registro2
 * \return 	: void
 * */
void guardarCalibracion(double input);
;
/**
 * \fn 		: double obtenerCalibracion();
 * \brief 	: Obtiene el valor guardado en un registro
 * \details : Reconstruye un dato de 64 bits a partir de leer 2 registros de 32
 * \author 	: Martin Rivas
 * \date   	: 23/10/2021
 * \param 	: [in]
 * \return 	: [double] retorna el valor guardado en el registro
 * */
double obtenerCalibracion();



#endif /* INC_REGISTER_H_ */
