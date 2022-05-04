/*
 * debounce.c
 *
 *  Created on: Jun 2, 2021
 *      Author: hernan
 */

#include "debounce.h"

void debounce_init(debounce_t *s, uint8_t active_low, uint16_t debounce){
	s->debounce = debounce;
	s->debounce_ticks = 0;
	s->active_low = (active_low)?1:0;
	s->state = 0;
    s->pin_state = (uint8_t)BTN_FREE;
	s->pin_prev_state = (uint8_t)BTN_HOLD;
}

void debounce_check(debounce_t *s, uint32_t state){
	int btn;
	btn = (state) ? 1 : 0;
	btn = btn ^ s->active_low;

	switch (s->state){
	case 0:
		if(btn){
			s->debounce_ticks = s->debounce;
			s->state = 1;
		}
		s->pin_state = BTN_FREE;
		s->pin_prev_state = BTN_FREE;
		break;
	case 1:
		if (!(--(s->debounce_ticks))){
			if(btn){
				s->pin_state = BTN_HOLD;
				s->pin_prev_state = BTN_FREE;
				s->state = 2;
			}else{
				s->state = 0;
			}
		}
		break;
	case 2:
		if(!btn){
			s->debounce_ticks = s->debounce;
			s->state = 3;
		}
		s->pin_state = BTN_HOLD;
		s->pin_prev_state = BTN_HOLD;
		break;
	case 3:
		if (!(--(s->debounce_ticks))){
			if(!btn){
				s->pin_state = BTN_FREE;
				s->pin_prev_state = BTN_HOLD;
				s->state = 0;
			}else{
				s->state = 2;
			}
		}
		break;
	}
}

int debounce_edge(debounce_t *s){
	return (s->pin_state == BTN_HOLD) && (s->pin_prev_state == BTN_FREE);
}

btn_state debounce_btn_state(debounce_t *s){
	return s->pin_state;
}
