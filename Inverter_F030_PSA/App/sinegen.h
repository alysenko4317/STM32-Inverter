#ifndef SINEGEN_H
#define SINEGEN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Number of points per full sine wave
#define SINE_SAMPLES    100

// Desired output frequency (Hz)
#define SINE_FREQ_HZ     50

// Total soft-ramp time (ms)
#define SOFT_MS       1000

// Computed update rate (Hz)
#define UPDATE_FREQ_HZ  (SINE_SAMPLES * SINE_FREQ_HZ)

// Number of ticks for ramping = UPDATE_FREQ_HZ * (SOFT_MS/1000)
#define RAMP_TICKS     ((UPDATE_FREQ_HZ * SOFT_MS) / 1000)

// Initialize sine generator (build table and configure TIM6)
void SineGen_Init(void);

// Start sine generation with soft-start ramp
void SineGen_Start(void);

// Initiate soft-stop ramp and stop when complete
void SineGen_Stop(void);

void SineGen_Update(void);

// TIM6 interrupt handler (hook into TIM6_DAC_IRQHandler)
//void TIM6_DAC_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // SINEGEN_H
