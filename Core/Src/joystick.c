/*
 * joystick_system.c
 *
 *  Created on: May 15, 2025
 *      Author: Sorro
 */

#include "system_state.h"
#include <stdlib.h>
void JOYSTICK_init(JoystickType* joy, ADC_HandleTypeDef* hadc) {
    joy->adc = hadc;
    joy->button_pressed = 0;
    joy->last_change_button = 0;
    joy->last_change_joystick = 0;

    // Change order of x and y depending on joystick layout
    joy->y_value = &hsystem_state.adc_buffer[0];
    joy->x_value = &hsystem_state.adc_buffer[1];
}

// Center = 0; Up = 1; Down = 2; Right = 3; Left = 4
uint8_t JOYSTICK_get_direction(JoystickType* joy) {
    int16_t x_diff = *joy->x_value - JOYSTICK_CENTER_X;
    int16_t y_diff = *joy->y_value - JOYSTICK_CENTER_Y;

    // Check deadzone
    if (abs(x_diff) < JOYSTICK_DEADZONE) x_diff = 0;
    if (abs(y_diff) < JOYSTICK_DEADZONE) y_diff = 0;

    // Determine direction
    if (x_diff == 0 && y_diff == 0) return 0; // No direction / centered

    if (abs(x_diff) > abs(y_diff)) {
        return (x_diff > 0) ? 3 : 4;
    } else {
        return (y_diff > 0) ? 1 : 2;
    }
}

void JOYSTICK_handle_interrupt_button(JoystickType* joy) {
	uint32_t tick_now = HAL_GetTick();

	if ((tick_now - joy->last_change_button) < JOYSTICK_DEBOUNCE_MS)
		return;

	joy->button_pressed = 1;
	joy->last_change_button = tick_now;
}

void JOYSTICK_process_event_button(JoystickType* joy, EventQueueType* evq){
	if(joy->button_pressed == 1) {
		EVENT_que_push_back(evq, ((Event){EVENT_JOYSTICK_PRESS, 0, NULL}));
		joy->button_pressed = 0;
	}
}


void JOYSTICK_process_event_joystick(JoystickType* joy, EventQueueType* evq){

	uint32_t tick_now = HAL_GetTick();
	if ((tick_now - joy->last_change_joystick) < JOYSTICK_RATE_LIMIT)
		return;

	uint8_t direction = JOYSTICK_get_direction(joy);

	if (direction) {
		joy->last_change_joystick = tick_now;

		EventType event = EVENT_ERROR;
		switch (direction) {
			case 1: event = EVENT_JOYSTICK_UP; break;
			case 2: event = EVENT_JOYSTICK_DOWN; break;
			case 3: event = EVENT_JOYSTICK_RIGHT; break;
			case 4: event = EVENT_JOYSTICK_LEFT; break;
			default: return;
        }

		EVENT_que_push_back(evq, (Event){event, 0, NULL});
    }
}


// Maybe move somewhere else or remake.
void JOYSTICK_handle_event(MenuType* menu, Event* event) {
	MenuInstanceType current_menu_instance = hsystem_state.current_menu->instance;
    if (menu != hsystem_state.current_menu) return;

    switch (event->type) {
        case EVENT_JOYSTICK_PRESS: {
        	// Has selectable menu
        	if(menu->item_count_sel <= 0)
        		return;

        	MenuItemType* current_item = &menu->menu_items_sel[menu->selected_index];
        	current_item->action_callback(current_item->in_action_callback);

        	// Update text has to be specified by the item callback ^

            break;
        }
        case EVENT_JOYSTICK_UP: {
        	if(current_menu_instance == MENU_TYPE_SEL_BPM || current_menu_instance == MENU_TYPE_SEL_SIG)
				break;

        	menu->selected_index = (menu->selected_index - 1 + menu->item_count_sel) % menu->item_count_sel;

        	MENU_render(menu, 0);
            break;
        }

        case EVENT_JOYSTICK_DOWN: {
        	if(current_menu_instance == MENU_TYPE_SEL_BPM || current_menu_instance == MENU_TYPE_SEL_SIG)
				break;

        	menu->selected_index = (menu->selected_index+1) % menu->item_count_sel;

        	MENU_render(menu, 0);
            break;
        }

        case EVENT_JOYSTICK_RIGHT: {
        	if(current_menu_instance == MENU_TYPE_SEL_BPM) {
        		++hsystem_state.current_bpm;
        	} else if (current_menu_instance == MENU_TYPE_SEL_SIG) {
        		hsystem_state.current_sig = (hsystem_state.current_sig == 4) ? 2 : hsystem_state.current_sig + 1;
        	} else
            	menu->selected_index = (menu->selected_index+1) % menu->item_count_sel;
        		//menu->selected_index = (menu->selected_index+2) % menu->item_count_sel;

        	MENU_render(menu, 0);
            break;
        }

        case EVENT_JOYSTICK_LEFT: {
        	if(current_menu_instance == MENU_TYPE_SEL_BPM) {
        		--hsystem_state.current_bpm;
        	} else if (current_menu_instance == MENU_TYPE_SEL_SIG) {
        		hsystem_state.current_sig = (hsystem_state.current_sig == 2) ? 4 : hsystem_state.current_sig - 1;
        	} else
            	menu->selected_index = (menu->selected_index - 1 + menu->item_count_sel) % menu->item_count_sel;
        		//menu->selected_index = (menu->selected_index - 2 + menu->item_count_sel) % menu->item_count_sel;

        	MENU_render(menu, 0);
            break;
        }

        default:
        	break;
    }
}
