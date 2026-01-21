/*
 * mic_sensor.h
 *
 *  Created on: May 20, 2025
 *      Author: Sorro
 */

#ifndef INC_MIC_SENSOR_H_
#define INC_MIC_SENSOR_H_

#include "system_state.h"

#define NOISE_SAMPLES 30    // Samples to establish noise floor
#define MIN_CLAP_GAP 100     // Minimum ms between claps (debounce)
#define SMOOTHING_FACTOR 1 // For moving average

typedef struct {
	uint16_t threshold;
	uint16_t* value;
    uint16_t noise_floor;    // Ambient noise level
    uint16_t peak_value;     // Current peak value
    uint32_t last_clap_time; // Timestamp of last clap
    float bpm;               // Calculated BPM
    float avg_interval;      // Average interval in ms
    uint8_t ready;           // Flag when BPM is valid
} MICType;


void MIC_init(MICType* mic);


uint8_t MIC_update_bpm(MICType* mic, uint32_t current_time);

void MIC_process_event(MICType* mic, EventQueueType* evq);

#endif /* INC_MIC_SENSOR_H_ */
