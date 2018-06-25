/*
 *  hal_pwm.h
 *
 *  Created on: 22-Jun-2018
 *
 *  Copyright (c) 2018, Appiko
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its contributors
 *  may be used to endorse or promote products derived from this software without
 *  specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 *  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CODEBASE_HAL_HAL_PWM_H_
#define CODEBASE_HAL_HAL_PWM_H_

#define HAL_PWM_MAX_PIN_NUM     4

/** @brief Select the PWM frequency to operate at. */
typedef enum
{
    HAL_PWM_FREQ_16MHz  = PWM_PRESCALER_PRESCALER_DIV_1,  /// 16 MHz / 1 = 16 MHz.
    HAL_PWM_FREQ_8MHz   = PWM_PRESCALER_PRESCALER_DIV_2,  /// 16 MHz / 2 = 8 MHz.
    HAL_PWM_FREQ_4MHz   = PWM_PRESCALER_PRESCALER_DIV_4,  /// 16 MHz / 4 = 4 MHz.
    HAL_PWM_FREQ_2MHz   = PWM_PRESCALER_PRESCALER_DIV_8,  /// 16 MHz / 8 = 2 MHz.
    HAL_PWM_FREQ_1MHz   = PWM_PRESCALER_PRESCALER_DIV_16, /// 16 MHz / 16 = 1 MHz.
    HAL_PWM_FREQ_500kHz = PWM_PRESCALER_PRESCALER_DIV_32, /// 16 MHz / 32 = 500 kHz.
    HAL_PWM_FREQ_250kHz = PWM_PRESCALER_PRESCALER_DIV_64, /// 16 MHz / 64 = 250 kHz.
    HAL_PWM_FREQ_125kHz = PWM_PRESCALER_PRESCALER_DIV_128 /// 16 MHz / 128 = 125 kHz.
} hal_pwm_freq_t;

/** @brief Select the operating mode of the PWM wave counter */
typedef enum
{
    /// Up counter, reset to 0 on incrementing to CounterTop
    HAL_PWM_MODE_UP      = PWM_MODE_UPDOWN_Up,
    /// Up & down counter, decrement back to 0 on reaching CounterTop
    HAL_PWM_MODE_UP_DOWN = PWM_MODE_UPDOWN_UpAndDown,
} hal_pwm_mode_t;

/**
 * @brief Bit masks for the PWM shortcuts. Or the appropriate ones for the
 *  required shortcuts.
 */
typedef enum
{
    /// Short between SEQEND[0] event and STOP task.
    HAL_PWM_SHORT_SEQEND0_STOP_MASK        = PWM_SHORTS_SEQEND0_STOP_Msk,
    /// Short between SEQEND[1] event and STOP task.
    HAL_PWM_SHORT_SEQEND1_STOP_MASK        = PWM_SHORTS_SEQEND1_STOP_Msk,
    /// Short between LOOPSDONE event and SEQSTART[0] task.
    HAL_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK = PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk,
    /// Short between LOOPSDONE event and SEQSTART[1] task.
    HAL_PWM_SHORT_LOOPSDONE_SEQSTART1_MASK = PWM_SHORTS_LOOPSDONE_SEQSTART1_Msk,
    /// Short between LOOPSDONE event and STOP task.
    HAL_PWM_SHORT_LOOPSDONE_STOP_MASK      = PWM_SHORTS_LOOPSDONE_STOP_Msk
} hal_pwm_short_mask_t;

/** @brief PWM decoder's data load modes. This mode selects how the different channels' next
 * value is loaded from the data read from the RAM. */
typedef enum
{
    /// A 16-bit value is used in all four (0-3) PWM channels.
    HAL_PWM_LOAD_COMMON     = PWM_DECODER_LOAD_Common,
    /// 1st 16-bit value used in channels 0 and 1; 2nd one in channels 2 and 3.
    HAL_PWM_LOAD_GROUPED    = PWM_DECODER_LOAD_Grouped,
    /// Each channel has its own 16 bit value.
    HAL_PWM_LOAD_INDIVIDUAL = PWM_DECODER_LOAD_Individual,
    /// 1st three channels have their own 16 bit values, 4th one is the wave counter.
    HAL_PWM_LOAD_WAVE_FORM  = PWM_DECODER_LOAD_WaveForm
} hal_pwm_decoder_load_t;

/** @brief PWM decoder's next load trigger modes. This selects when the next value is
 *  loaded from RAM. */
typedef enum
{
    /// After the loaded value is repeated for the number of times in REFRESH register
    HAL_PWM_STEP_INTERNAL = PWM_DECODER_MODE_RefreshCount,
    /// When the Next Step task is set
    HAL_PWM_STEP_EXTERNAL = PWM_DECODER_MODE_NextStep
} hal_pwm_dec_trigger_t;

/** @brief Configuration for a sequence of PWM values. Note that the buffer pointed to
 *  in this structure needs to have a lifetime longer than the duration of the PWM.
 */
typedef struct
{
    /// Pointer to an array containing the PWM duty cycle values.
    /// This array present in the data RAM should preferably have a
    /// program lifetime (global or local static variable).
    uint16_t * seq_values;
    /// Number of 16-bit values in the buffer pointed by @p seq_values.
    uint16_t len;
    /// Number of times a particular value should be played minus one.
    /// Only for @ref HAL_PWM_STEP_INTERNAL mode.
    uint32_t repeats;
    /// Additional number of cycles that the last PWM value is to be played after the end.
    /// Only for @ref HAL_PWM_STEP_INTERNAL mode.
    uint32_t end_delay;
} hal_pwm_sequence_config_t;

/**
 * @brief PWM interrupts.
 */
typedef enum
{
    /// Interrupt on STOPPED event.
    HAL_PWM_IRQ_STOPPED_MASK      = PWM_INTENSET_STOPPED_Msk,
    /// Interrupt on SEQSTARTED[0] event.
    HAL_PWM_IRQ_SEQSTARTED0_MASK  = PWM_INTENSET_SEQSTARTED0_Msk,
    /// Interrupt on SEQSTARTED[1] event.
    HAL_PWM_IRQ_SEQSTARTED1_MASK  = PWM_INTENSET_SEQSTARTED1_Msk,
    /// Interrupt on SEQEND[0] event.
    HAL_PWM_IRQ_SEQEND0_MASK      = PWM_INTENSET_SEQEND0_Msk,
    /// Interrupt on SEQEND[1] event.
    HAL_PWM_IRQ_SEQEND1_MASK      = PWM_INTENSET_SEQEND1_Msk,
    /// Interrupt on PWMPERIODEND event.
    HAL_PWM_IRQ_PWMPERIODEND_MASK = PWM_INTENSET_PWMPERIODEND_Msk,
    /// Interrupt on LOOPSDONE event.
    HAL_PWM_IRQ_LOOPSDONE_MASK    = PWM_INTENSET_LOOPSDONE_Msk
} hal_pwm_irq_mask_t;

/**
 *
 */
typedef struct
{
    uint32_t * pins;
    uint32_t pin_num;
    hal_pwm_freq_t oper_freq;
    hal_pwm_mode_t oper_mode;
}hal_pwm_init_t;

/**
 *
 */
typedef struct
{
    uint32_t countertop;
    uint32_t loop;
    uint32_t shorts_mask;
    uint32_t interrupt_masks;
    hal_pwm_decoder_load_t decoder_load;
    hal_pwm_dec_trigger_t decoder_trigger;
    hal_pwm_sequence_config_t seq_config[2];
    void (*irq_handler)(void);
}hal_pwm_start_t;

/**
 *
 * @param init_config
 */
void hal_pwm_init(hal_pwm_init_t init_config);

/**
 *
 * @param start_config
 */
void hal_pwm_start(hal_pwm_start_t start_config);

/**
 *
 */
void hal_pwm_stop(void);

#endif /* CODEBASE_HAL_HAL_PWM_H_ */
