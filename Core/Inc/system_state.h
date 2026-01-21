/*
 * system_state.h
 *
 *  Created on: May 14, 2025
 *      Author: Sorro
 */

#ifndef INC_SYSTEM_STATE_H_
#define INC_SYSTEM_STATE_H_

#include "event_system.h"
#include "lcd.h"
#include "menu_system.h"
#include "joystick.h"
#include "mic_sensor.h"
#include "rhythm_controller.h"

#define ADC_BUF_SIZE 3

typedef struct{
	MenuType* current_menu;
	MenuType* previous_menu;
	uint8_t current_bpm;
	uint8_t current_sig;
	uint8_t accuracy;
	uint8_t reaction;
	uint8_t current_beat;
	TextLCDType hlcd;
	EventQueueType hevq;
	JoystickType hjoy;
	MICType hmic;
	RhythmController hrhythm;
	uint16_t adc_buffer[ADC_BUF_SIZE];
} SystemState;

extern SystemState hsystem_state;

#endif /* INC_SYSTEM_STATE_H_ */
