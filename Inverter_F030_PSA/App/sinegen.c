/**
 * @file sinegen.c
 * @brief Sinusoidal PWM generator for inverter bridge using STM32 timers.
 *
 * Configuration summary:
 * - TIM16 and TIM17: high-frequency carrier PWM timers (e.g. 16 kHz),
 *   generating PWM on CH1 and CH1N outputs to drive MOSFET bridge legs.
 * - TIM6: update timer running at UPDATE_FREQ_HZ (e.g. 100 samples * 50 Hz = 5 kHz),
 *   used to modulate TIM16/TIM17 duty cycle according to sine lookup table.
 *
 * Features:
 * - Soft-start ramp: duty amplitude increases smoothly from 0 to 100% over SOFT_MS ms.
 * - Soft-stop ramp: duty amplitude decreases back to 0 over SOFT_MS ms, then disables bridge.
 * - All HAL TIM6 start/stop handled inside module; main() calls only SineGen_Init(),
 *   SineGen_Start(), and SineGen_Stop().
 */

#include "tim.h"              // for timer externals
#include "gpio.h"             // for debug LEDs
#include "sinegen.h"
#include "stm32f0xx_hal.h"    // device register definitions

#include <math.h>

// Internal sine lookup table
static uint16_t sine_table[SINE_SAMPLES];

// Soft-ramp state
static volatile uint32_t sine_idx;
static volatile float    amplitude;
static float             amp_step_up;
static float             amp_step_down;

// Forward declarations
static void Bridge_Start(void);
static void Bridge_Stop(void);
static void Build_Sine_Table(void);

//-------------------------------------------------------------------------
// Functions controlling the bridge hardware.
//-------------------------------------------------------------------------

static void Bridge_Start(void)
{
    // Disable interrupts to keep TIM16 and TIM17 aligned:
    // These advanced-control timers lack a hardware master/slave chaining
    // mode on this STM32 series, so we gate their entire start sequence
    // in one atomic block.  Even if we ended up a few CPU cycles (≈10–20 ns)
    // apart, that skew is minute compared to the ~60 µs PWM period and
    // won’t affect bridge operation.
    __disable_irq();

    // 1) latch PSC/ARR/CCR preload registers
    TIM16->EGR = TIM_EGR_UG;
    TIM17->EGR = TIM_EGR_UG;

    // 2) reset both counters to 0
    TIM16->CNT = 0;
    TIM17->CNT = 0;

    // 3) enable channel outputs (main + complementary)
    TIM16->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;  // enable CH1 & CH1N
    TIM17->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

    // 4) enable the main outputs (MOE bit in BDTR)
    TIM16->BDTR |= TIM_BDTR_MOE;
    TIM17->BDTR |= TIM_BDTR_MOE;

    // 5) start both timers (CEN bit)
    TIM16->CR1 |= TIM_CR1_CEN;
    TIM17->CR1 |= TIM_CR1_CEN;

    __enable_irq();
}

void Bridge_Stop(void)
{
    __disable_irq();

    // 1) gate off the outputs immediately
    TIM16->BDTR &= ~TIM_BDTR_MOE;
    TIM17->BDTR &= ~TIM_BDTR_MOE;

    // 2) stop counting
    TIM16->CR1  &= ~TIM_CR1_CEN;
    TIM17->CR1  &= ~TIM_CR1_CEN;

    // 3) optionally disable the CCx outputs entirely
    TIM16->CCER &= ~(TIM_CCER_CC1E   | TIM_CCER_CC1NE);
    TIM17->CCER &= ~(TIM_CCER_CC1E   | TIM_CCER_CC1NE);

    // 4) optionally clear the counters so next start is clean
    TIM16->CNT = 0;
    TIM17->CNT = 0;

    __enable_irq();
}

//-------------------------------------------------------------------------
// Bridge Soft-Start (Debug/Experimental)
//-------------------------------------------------------------------------

// This routine is provided purely as an experiment and for debug purposes.
// It ramps the bridge PWM duty from 0% up to 50% over a fixed time window,
// in case you need a manual soft-start without the full sine-modulation logic.
// You can remove it once you’ve validated the main SineGen soft-start.
//
// The main SineGen module already implements a smoother, sine-based soft-start,
// so this is optional “just in case” code — feel free to drop it if unused.

#define SOFTSTART_STEPS   100    // How many discrete steps in our ramp
#define SOFTSTART_TIME_MS 1000   // Total ramp time in milliseconds

static inline void delay_ms(uint32_t ms)
{
    // assuming SysTick is ticking 1ms or you can implement your own busy-wait here
    extern void HAL_Delay(uint32_t);
    HAL_Delay(ms);
}

void Bridge_SoftStart(void)
{
    // 1) Make sure compare is zero so we start from 0% duty
    TIM16->CCR1 = 0;
    TIM17->CCR1 = 0;

    // 2) Fire up the timers and outputs synchronously
    Bridge_Start();

    // 3) Compute our target CCR (here: 50% duty = ARR/2)
    uint32_t target = (TIM16->ARR + 1) / 2;

    // 4) Ramp in SOFTSTART_STEPS discrete increments
    uint32_t delay_per_step = SOFTSTART_TIME_MS / SOFTSTART_STEPS;
    for (uint32_t step = 1; step <= SOFTSTART_STEPS; ++step)
    {
        uint32_t ccr = target * step / SOFTSTART_STEPS;
        TIM16->CCR1 = ccr;
        TIM17->CCR1 = ccr;
        delay_ms(delay_per_step);
    }
}

//-------------------------------------------------------------------------
// Sine Generation Module
//-------------------------------------------------------------------------
//
// This module produces a sinusoidal PWM drive for an inverter bridge
// using three STM32 timers:
//
//  • TIM16 & TIM17: High-frequency carrier PWM generators (e.g. 16 kHz),
//    driving both main (CH1) and complementary (CH1N) MOSFET outputs.
//  • TIM6:  Sample-rate timer running at UPDATE_FREQ_HZ (e.g. 100 samples × 50 Hz = 5 kHz),
//    which modulates the carrier duty according to a sine lookup table.
//
// Key features:
//  – Soft-start: smoothly ramps amplitude from 0→100% over SOFT_MS ms.
//  – Soft-stop: ramps amplitude back to 0 and then disables the bridge.
//  – All TIM6 start/stop is managed internally by SineGen_Start()/SineGen_Stop().
//  – Bridge control (Bridge_Start/Bridge_Stop) is hidden inside the module.
//
// Public API:
//   void SineGen_Init(void);   // Build lookup table, prepare timers (TIM6 configured in CubeMX)
//   void SineGen_Start(void);  // Begin soft-start, enable TIM6 interrupts
//   void SineGen_Stop(void);   // Begin soft-stop; bridge off when amplitude hits zero
//
//-------------------------------------------------------------------------

// Build a 0..ARR sine table scaled to timer ARR
static void Build_Sine_Table(void)
{
    const uint32_t arr = TIM16->ARR + 1;
    for (int i = 0; i < SINE_SAMPLES; i++) {
        float theta = 2.0f * 3.14159265f * i / (float)SINE_SAMPLES;
        float v = (sinf(theta) * 0.5f + 0.5f) * arr;
        sine_table[i] = (uint16_t)(v + 0.5f);
    }
}

void SineGen_Init(void)
{
    // Only build sine table; TIM6 is configured via CubeMX (code generated in main.c)
    Build_Sine_Table();
}

// Start sine generation with soft-start ramp and enables TIM6 interrupt
void SineGen_Start(void)
{
    // Reset state
    sine_idx      = 0;
    amplitude     = 0.0f;
    amp_step_up   = 1.0f / (float)RAMP_TICKS;
    amp_step_down = -amp_step_up;

    // Ensure starting duty = 0 to avoid initial 50% pulses
    TIM16->CCR1 = 0;
    TIM17->CCR1 = 0;

    // Start TIM16 and TIM17 and enable bridge gates PWM outputs
    Bridge_Start();

    // Start TIM6 interrupts for modulation
    HAL_TIM_Base_Start_IT(&htim6);
}

void SineGen_Stop(void)
{
    // Switch to descending ramp; actual stop and timer disable happens in update
    amp_step_up     = 0.0f;
    amp_step_down   = -(1.0f / (float)RAMP_TICKS);
}

void SineGen_Update(void)
{
    // advance amplitude
    amplitude += (amp_step_up != 0.0f ? amp_step_up : amp_step_down);
    if (amplitude >= 1.0f) {
        amplitude = 1.0f;
    } else if (amplitude <= 0.0f) {
        amplitude = 0.0f;
        // Stop TIM6 interrupt and bridge
        HAL_TIM_Base_Stop_IT(&htim6);
        Bridge_Stop();
        return;
    }

    // get next sample
    uint16_t raw = sine_table[sine_idx++];
    if (sine_idx >= SINE_SAMPLES)
        sine_idx = 0;

    // scale and write
    uint16_t ccr = (uint16_t)(raw * amplitude);
    TIM16->CCR1 = ccr;
    TIM17->CCR1 = ccr;
}

// Hook into HAL's period-elapsed callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    LED_A_Toggle();  // Debug LED

    if (htim->Instance == TIM6) {
        SineGen_Update();
    }
}

