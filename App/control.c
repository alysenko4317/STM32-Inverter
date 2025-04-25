#include "app.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief  Takes sensorPacket_t from xSensorQueue, computes control level,
 *         and sends it to xControlQueue.
 */
extern QueueHandle_t xSensorQueue;
extern QueueHandle_t xControlQueue;

void ControlTask(void *arg)
{
    sensorPacket_t pkt;
    control_t      ctrl;
    for (;;)
    {
        if (xQueueReceive(xSensorQueue, &pkt, portMAX_DELAY) == pdPASS)
        {
            // Example: map raw[0]..raw[1] to level 0..1
            ctrl.level = pkt.raw[0] / 255.0f;
            xQueueOverwrite(xControlQueue, &ctrl);
        }
    }
}
