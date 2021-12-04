/*
 * debounce.h
 *
 *  Created on: Jun 2, 2021
 *      Author: hernan
 */

#ifndef INC_OLED_BALANZA_H_
#define INC_OLED_BALANZA_H_


int count_digits(int number);


void printoled_battery(int state);

void printoled_menu();

void printoled_weight(int weight,int unit);

void printoled_start();

void printoled_number(int number);

void printoled_msg(int text);

void printoled_price(float price, int mode);

void printoled_PC(int text);

void printoled_tare();

#endif /* INC_DEBOUNCE_H_ */
