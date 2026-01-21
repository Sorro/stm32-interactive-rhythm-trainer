/*
 * rhythm_controller.h
 *
 *  Created on: May 20, 2025
 *      Author: Sorro
 */

#ifndef INC_RHYTHM_CONTROLLER_H_
#define INC_RHYTHM_CONTROLLER_H_

#define RGB_RED_CHANNEL    TIM_CHANNEL_2
#define RGB_GREEN_CHANNEL  TIM_CHANNEL_4
#define RGB_BLUE_CHANNEL   TIM_CHANNEL_3
#define BUZZER_CHANNEL     TIM_CHANNEL_1

extern TIM_HandleTypeDef htim1;
#define PWM_TIMER         &htim1

// Beat parameters
#define BEAT_ANIMATION_DURATION 100  // ms
#define BUZZER_PULSE_MS 20           // Buzzer duration per beat

#define FADE_DURATION 80  // ms for fade-out effect
#define YELLOW_RED_RATIO 0.7f  // Red component in yellow mix

typedef struct {
    uint32_t last_beat_time;

    // Beat Tracking
    uint8_t current_beat;
    uint16_t intensity;  // For progressive yellow

    // Output States
    uint32_t animation_end;
    uint32_t buzzer_off_time;
} RhythmController;

void RHYTHM_init(RhythmController* rc);

void RHYTHM_update(RhythmController* rc, uint32_t current_time, uint8_t current_bpm, uint8_t current_sig, uint8_t buzzer);

#endif /* INC_RHYTHM_CONTROLLER_H_ */
