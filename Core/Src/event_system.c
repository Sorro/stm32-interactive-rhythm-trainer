/*
 * event_system.c
 *
 *  Created on: May 15, 2025
 *      Author: Sorro
 */

#include "system_state.h"

void EVENT_que_init(EventQueueType* evq) {
	for (int i = 0; i < EVENT_QUEUE_CAPACITY; ++i)
	    evq->que[i] = (Event){EVENT_ERROR};

	// Perhaps make array on heap in future?
	evq->que_size = 0;
	evq->que_ix_front = 0;
}

uint8_t EVENT_que_ix_back(EventQueueType* evq) {
	return (evq->que_ix_front + evq->que_size-1) % EVENT_QUEUE_CAPACITY;
}

uint8_t EVENT_que_ix_next_back(EventQueueType* evq) {
	return (evq->que_ix_front + evq->que_size) % EVENT_QUEUE_CAPACITY;
}

uint8_t EVENT_que_ix_next_front(EventQueueType* evq) {
	return (evq->que_ix_front - 1 + evq->que_size) % EVENT_QUEUE_CAPACITY;
}

Event* EVENT_que_front(EventQueueType* evq) {
	return &evq->que[evq->que_ix_front];
}

bool EVENT_que_push_back(EventQueueType* evq, Event event) {
	if(evq->que_size >= EVENT_QUEUE_CAPACITY)
		return false; // If this turns to be a more serious project, perhaps handle resizing.

    // Fyll nästa lediga position bakom sista elementet med data.
    evq->que[EVENT_que_ix_next_back(evq)] = event;

    // Korrigera storleks variabeln.
    ++evq->que_size;

    return true;
}

EventType EVENT_que_pop_front(EventQueueType* evq) {
	EventType e_return = EVENT_ERROR;

	// Arrayen ska ej vara tom.
	if(evq->que_size > 0) {

		e_return = EVENT_que_front(evq)->type;

		// Pop
		evq->que[evq->que_ix_front].type = EVENT_ERROR;

		// Sätt index till 0 ifall elementet som "gick" ut var på slutet av arrayen, n-1.
		// Eller om den är det enda elementet i arrayen.
		if(evq->que_ix_front == EVENT_QUEUE_CAPACITY-1 || evq->que_size == 1)
			evq->que_ix_front= 0;
		// Annars, öka index med 1.
		else
			++evq->que_ix_front;

		// Korrigera storleks variabeln.
		--evq->que_size;
    }

    return e_return;
}


// Event handlers - perhaps move to seperate class for SRP.
void EVENT_handle(Event* event) {
	switch(event->type) {
		// Joystick Events
		case EVENT_JOYSTICK_UP:
		case EVENT_JOYSTICK_DOWN:
		case EVENT_JOYSTICK_LEFT:
		case EVENT_JOYSTICK_RIGHT:
		case EVENT_JOYSTICK_PRESS:
			EVENT_handle_joystick_event(event);
			break;

		// BPM Events
		case EVENT_BPM_CHANGED:
		case EVENT_BPM_BEAT:
		case EVENT_BPM_START:
		case EVENT_BPM_STOP:
			EVENT_handle_bpm_event(event);
			break;

		// Sound detection events
		case EVENT_SOUND_DETECTED:
		case EVENT_SOUND_THRESHOLD_CHANGED:
			EVENT_handle_sound_event(event);
			break;

		// System events
		case EVENT_MENU_CHANGE_REQUEST:
		case EVENT_UPDATE_DISPLAY_TEXT:
			EVENT_handle_menu_event(event);
			break;

		default:
			break;
	}
}

void EVENT_handle_joystick_event(Event* event) {
	JOYSTICK_handle_event(hsystem_state.current_menu, event);
}

void EVENT_handle_bpm_event(Event* event) {
	switch(event->type) {
		// Sound detection events
		case EVENT_BPM_CHANGED:
			EVENT_que_push_back(&hsystem_state.hevq, (Event){EVENT_UPDATE_DISPLAY_TEXT, 0, NULL});
			//RHYTHM_update(&hsystem_state.hrhythm, HAL_GetTick(), hsystem_state.current_bpm, hsystem_state.current_sig, 0);
		default: break;
	}
}

void EVENT_handle_sound_event(Event* event){
	switch(event->type) {
		// Sound detection events
		case EVENT_SOUND_DETECTED:
			if(hsystem_state.current_menu->instance == MENU_TYPE_BPM_DETECT) {
				hsystem_state.current_bpm = hsystem_state.hmic.bpm;
				EVENT_que_push_back(&hsystem_state.hevq, (Event){EVENT_BPM_CHANGED, 0, NULL});
			}
		case EVENT_SOUND_THRESHOLD_CHANGED:
			break;

		default: break;
	}
}

void EVENT_handle_menu_event(Event* event) {
	switch(event->type) {
		// Sound detection events
		case EVENT_UPDATE_DISPLAY_TEXT:
			if(hsystem_state.current_menu->instance == MENU_TYPE_BPM_DETECT)
				MENU_item_render_text_ix(hsystem_state.current_menu, 0, 0);
			else if(hsystem_state.current_menu->instance == MENU_TYPE_BPM_PRACTICE)
				MENU_item_render_text_ix(hsystem_state.current_menu, 0, 1);
		case EVENT_MENU_CHANGE_REQUEST:
			//Maybe unneccesary?
			break;

		default: break;
	}
}

//TODO
/*void EVENT_handle_error(Event* event) {

}*/
