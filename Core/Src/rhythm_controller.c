/*
 * rhythm_controller.c
 *
 *  Created on: May 20, 2025
 *  Author: Sorro
 */

#include "system_state.h"

void RHYTHM_init(RhythmController* rc) {
    rc->last_beat_time = 0;
    rc->current_beat = 0;
    rc->intensity = 0;
    rc->animation_end = 0;
    rc->buzzer_off_time = 0;

    // Start all PWM channels
    HAL_TIM_PWM_Start(PWM_TIMER, RGB_RED_CHANNEL);
    HAL_TIM_PWM_Start(PWM_TIMER, RGB_GREEN_CHANNEL);
    HAL_TIM_PWM_Start(PWM_TIMER, RGB_BLUE_CHANNEL);
    HAL_TIM_PWM_Start(PWM_TIMER, BUZZER_CHANNEL);

    // Turn all outputs off initially
    __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_RED_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_GREEN_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_BLUE_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(PWM_TIMER, BUZZER_CHANNEL, 0);
}

void RHYTHM_update(RhythmController* rc, uint32_t current_time, uint8_t current_bpm, uint8_t current_sig, uint8_t buzzer) {
    // Safeguards and initialization
    if(current_bpm == 0) current_bpm = 120;
    if(current_sig == 0) current_sig = 4;

    uint32_t beat_interval = 60000 / current_bpm;

    // Check if beat should trigger
    if((current_time - rc->last_beat_time) >= beat_interval) {
        rc->last_beat_time = current_time;
        rc->current_beat = (rc->current_beat + 1) % current_sig;
        rc->animation_end = current_time + BEAT_ANIMATION_DURATION;

        // First beat - green at 50% intensity
        if(rc->current_beat == 0) {
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_RED_CHANNEL, 0);
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_GREEN_CHANNEL, 4999); // 50% of 1000
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_BLUE_CHANNEL, 0);
        }
        // Subsequent beats - increasing yellow (red+green mix)
        else {
            // Calculate intensity ramp (0.3 to 1.0 range)
            float progress = (float)rc->current_beat / (current_sig - 1);
            uint16_t intensity = 499 + (1000 * progress); // 30% to 100%

            // Yellow = Red + Green (with your specified scaling)
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_RED_CHANNEL, intensity);
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_GREEN_CHANNEL, 0); // 50% of intensity
            __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_BLUE_CHANNEL, intensity * 0.5f); // 80% of intensity
        }

        // Handle buzzer
        if(buzzer) {
            __HAL_TIM_SET_COMPARE(PWM_TIMER, BUZZER_CHANNEL, 2500);
            rc->buzzer_off_time = current_time + BUZZER_PULSE_MS;
        }
    }

    // Turn off LEDs when animation period ends
    if(current_time > rc->animation_end) {
        __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_RED_CHANNEL, 0);
        __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_GREEN_CHANNEL, 0);
        __HAL_TIM_SET_COMPARE(PWM_TIMER, RGB_BLUE_CHANNEL, 0);
    }

    // Turn off buzzer after pulse duration
    /*if(current_time > rc->buzzer_off_time) {
        __HAL_TIM_SET_COMPARE(PWM_TIMER, BUZZER_CHANNEL, 0);
    }*/
}
