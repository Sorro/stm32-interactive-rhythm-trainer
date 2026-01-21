/*
 * joystick_system.h
 *
 *  Created on: May 15, 2025
 *      Author: Sorro
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include <stdint.h>

#include "main.h"

#include "system_state.h"

#define JOYSTICK_CENTER_X 2048
#define JOYSTICK_CENTER_Y 2048
#define JOYSTICK_DEADZONE 350
#define JOYSTICK_DEBOUNCE_MS 200
#define JOYSTICK_RATE_LIMIT 200

typedef struct {
	ADC_HandleTypeDef* adc;

	// Change back order x value is first and not y (For DMA)
    uint16_t* y_value;
    uint16_t* x_value;

    uint8_t button_pressed;

    uint32_t last_change_button;
    uint32_t last_change_joystick;
} JoystickType;

void JOYSTICK_init(JoystickType* joy, ADC_HandleTypeDef* hadc);

// Center = 0; Up = 1; Down = 2; Right = 3; Left = 4
uint8_t JOYSTICK_get_direction(JoystickType* joy);

void JOYSTICK_handle_interrupt_button(JoystickType* joy);

void JOYSTICK_process_event_button(JoystickType* joy, EventQueueType* evq);
void JOYSTICK_process_event_joystick(JoystickType* joy, EventQueueType* evq);


void JOYSTICK_handle_event(MenuType* menu, Event* event);

#endif /* INC_JOYSTICK_H_ */
