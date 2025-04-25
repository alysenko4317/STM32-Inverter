
#include "app.h"
#include "FreeRTOS.h"         // for xQueuePeek, pdPASS
#include "queue.h"            // for xQueuePeek
#include "cmsis_os2.h"        // for osDelay
#include "stm324xg_eval_lcd.h"
#include "stm324xg_eval.h"
#include <stdio.h>

extern QueueHandle_t xSensorQueue;

/**
 * @brief  Blink LED1 and refresh display with last sensor packet.
 */

void LEDUITask(void *argument)
{
    (void)argument;

    // Optional: set up text/font once
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font24);

    for (;;)
    {
        // Toggle the heartbeat LED
        BSP_LED_Toggle(LED1);

        // Clear the line to avoid leftover pixels
        BSP_LCD_ClearStringLine(1);

        // Print on line 1 (second text line)
        BSP_LCD_DisplayStringAtLine(1, (uint8_t*)"LEDUITask");

        // Block for 500 ms so other tasks (and you!) can see it
        osDelay(500);
    }
}

/*
void LEDUITask(void *arg)
{
    sensorPacket_t pkt;
    char buf[40];

    BSP_LCD_DisplayStringAtLine(0, (uint8_t*)"LEDUITask");

    for (;;)
    {
        BSP_LED_Toggle(LED1);

        if (xQueuePeek(xSensorQueue, &pkt, 0) == pdPASS)
        {
            // format and display 9 values as 3-digit, space-padded
            char *p = buf;
            for (int i = 0; i < 9; i++)
            {
                p += sprintf(p, "%3u ", pkt.raw[i]);
            }
            *--p = '\0'; // remove trailing space

            BSP_LCD_ClearStringLine(2);
            BSP_LCD_DisplayStringAtLine(2, (uint8_t*)buf);
        }

        osDelay(200);
    }
}
*/
