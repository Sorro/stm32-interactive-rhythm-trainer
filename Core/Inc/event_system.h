/*
 * events.h
 *
 *  Created on: May 14, 2025
 *      Author: Sorro
 */

#ifndef INC_EVENT_SYSTEM_H_
#define INC_EVENT_SYSTEM_H_

#include <stdint.h>
#include "stdbool.h"

typedef enum {
	// Joystick Events
	EVENT_JOYSTICK_UP,
	EVENT_JOYSTICK_DOWN,
	EVENT_JOYSTICK_LEFT,
	EVENT_JOYSTICK_RIGHT,
	EVENT_JOYSTICK_PRESS,

	// BPM Events
	EVENT_BPM_CHANGED,
	EVENT_BPM_BEAT,
	EVENT_BPM_START,
	EVENT_BPM_STOP,

	// Sound detection events
	EVENT_SOUND_DETECTED,
	EVENT_SOUND_THRESHOLD_CHANGED,

	// System events
	EVENT_MENU_CHANGE_REQUEST,
	EVENT_UPDATE_DISPLAY_TEXT,
	EVENT_ERROR
} EventType;

typedef struct {
    EventType type;
    uint16_t simple_data;   // For simple values (BPM, thresholds, etc.)
    void* complex_data;     // For complex data or pointers
} Event;

#define EVENT_QUEUE_CAPACITY 50

typedef struct {
	Event que[EVENT_QUEUE_CAPACITY];
	uint8_t que_size;
	uint8_t que_ix_front;
} EventQueueType;

// Event Array Queue system

uint8_t EVENT_que_ix_back(EventQueueType* evq);
uint8_t EVENT_que_ix_next_back(EventQueueType* evq);
uint8_t EVENT_que_ix_next_front(EventQueueType* evq);

Event* EVENT_que_front(EventQueueType* evq);

void EVENT_que_init(EventQueueType* evq);

bool EVENT_que_push_back(EventQueueType* evq, Event event);
EventType EVENT_que_pop_front(EventQueueType* evq);

// Event handlers

void EVENT_handle(Event* event);

void EVENT_handle_joystick_event(Event* event);
void EVENT_handle_bpm_event(Event* event);
void EVENT_handle_sound_event(Event* event);
void EVENT_handle_menu_event(Event* event);

#endif /* INC_EVENT_SYSTEM_H_ */
