/*
 * mic_sensor.c
 *
 *  Created on: May 15, 2025
 *  Author: Sorro
 */

#include "system_state.h"

void MIC_init(MICType* mic) {
	mic->threshold = 0;
	mic->value = &hsystem_state.adc_buffer[2];
    mic->noise_floor = 0;
    mic->peak_value = 0;
    mic->last_clap_time = 0;
    mic->bpm = 0;
    mic->avg_interval = 0;
    mic->ready = 0;
}

uint8_t MIC_update_bpm(MICType* mic, uint32_t current_time) {
    uint16_t current = *mic->value;

    // 1. Establish noise floor
    static uint16_t sample_count = 0;
    if (sample_count < NOISE_SAMPLES) {
        mic->noise_floor = (mic->noise_floor * sample_count + current) / (sample_count + 1);
        sample_count++;
        return 0;
    }

    // 2. Dynamic threshold (exponential moving average)
    mic->threshold = 0.9f * mic->threshold + 0.1f * mic->noise_floor;

    // 3. Peak detection
    if (current > mic->threshold + 50) {
        if (current > mic->peak_value) {
            mic->peak_value = current;
        }
    }
    else if (mic->peak_value > 0) {
        // 4. Falling edge detected
        if (mic->last_clap_time > 0 &&
            (current_time - mic->last_clap_time) > MIN_CLAP_GAP) {

            // Calculate new interval
            float new_interval = current_time - mic->last_clap_time;

            // Update moving average
            if (mic->avg_interval == 0) {
                mic->avg_interval = new_interval;
            } else {
                mic->avg_interval = SMOOTHING_FACTOR * new_interval +
                                  (1 - SMOOTHING_FACTOR) * mic->avg_interval;
            }

            // Calculate BPM (60000 ms per minute / interval in ms)
            mic->bpm = 60000.0f / mic->avg_interval;
            mic->ready = 1;
        }

        mic->last_clap_time = current_time;
        mic->peak_value = 0;
        return 1; // Clap detected
    }

    return 0;
}

// TODO later if I want to seperate responsibility.
/*void MIC_update_threshold(MICType* mic) {


}*/

void MIC_process_event(MICType* mic, EventQueueType* evq) {
    if (MIC_update_bpm(&hsystem_state.hmic, HAL_GetTick())) {
		EVENT_que_push_back(evq, (Event){EVENT_SOUND_DETECTED, 0, NULL});    }
}
