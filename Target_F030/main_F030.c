#include "main.h"
#include "bsp_f0.h"
#include "sensor.h"
#include "comms.h"
#include "sinegen.h"
#include "led.h"

/**
 * @brief  Entry point for F030 (bare-metal).
 *         Sets up board, then runs super-loop for comms, control & PWM.
 */
int main(void)
{
    // Initialize board clocks, GPIO, ADC, TIM, UART
    BSP_BoardInit();

    // Initialize modules
    SineGen_InitTable();
    LED_Init();
    Comms_Init();      // sets up UART3 IRQ, ring buffer
    ADC_StartDMA();    // if using DMA, else HAL_ADC_Start_IT()

    // Main super-loop
    while (1)
    {
        // 1) Parse any complete UART packets
        sensorPacket_t pkt;
        while (TryParsePacket(&pkt))
        {
            ProcessIncomingData(&pkt);  // update sensor values
        }

        // 2) Update PWM outputs
        SineGen_Update();  // updates CCR registers with ramped sine

        // 3) Toggle heartbeat LED
        LED_ToggleHeartbeat();

        // 4) Optional small delay to pace the loop
        HAL_Delay(1);
    }
}
