/* Sine generation task */

#include "app.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"

#include <math.h>

static uint16_t sineTable[100];
static float    ramp = 0, rampStep = 0;

extern QueueHandle_t xControlQueue;

/**
 * @brief  Initializes sine lookup table and starts PWM channels.
 */
void SineGenTask(void *arg)
{
    // fill table once
    for (int i = 0; i < 100; i++)
        sineTable[i] = (uint16_t)((sinf(2*M_PI*i/100)*0.5f+0.5f) * __HAL_TIM_GET_AUTORELOAD(&htim1));

    uint32_t idx = 0;
    control_t ctrl;

    for (;;)
    {
        // fetch latest control level if available
        if (xQueueReceive(xControlQueue, &ctrl, 0) == pdPASS) {
            ramp     = ctrl.level;
            rampStep = 0;  // optionally adjust ramp over time
        }

        // update PWM duty
        idx = (idx + 1) % 100;
        uint16_t duty = (uint16_t)(sineTable[idx] * ramp);
        for (int ch = TIM_CHANNEL_1; ch <= TIM_CHANNEL_4; ch <<= 1)
            __HAL_TIM_SET_COMPARE(&htim1, ch, duty);

        osDelay(1);  // 1 ms tick
    }
}
