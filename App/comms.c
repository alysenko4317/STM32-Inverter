/* HV MCU reading task */

#include "app.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os.h"
#include "stm324xg_eval.h"
#include "stm324xg_eval_lcd.h"

#include <stdbool.h>

extern QueueHandle_t xSensorQueue;

#define UART_BUF_SIZE 128
static uint8_t uart_buf[UART_BUF_SIZE];
static volatile size_t uart_head, uart_tail;

void USART3_IRQHandler(void)
{
  uint8_t b;
  if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))
  {
    b = (uint8_t)(huart3.Instance->DR & 0xFF);
    uart_buf[uart_head++] = b;
    if (uart_head >= UART_BUF_SIZE) uart_head = 0;
  }
  HAL_UART_IRQHandler(&huart3);
}

static bool TryParsePacket(uint8_t *out)
{
  // проверяем, что в буфере >=10 байт
  size_t cnt = (uart_head + UART_BUF_SIZE - uart_tail) % UART_BUF_SIZE;
  if (cnt < 10) return false;

  // читаем 9 байт + 0x55 в checksum
  uint8_t cs = 0x55;
  for (int i = 0; i < 9; i++)
  {
    uint8_t b = uart_buf[(uart_tail + i) % UART_BUF_SIZE];
    cs ^= b;
  }
  uint8_t pkt_cs = uart_buf[(uart_tail + 9) % UART_BUF_SIZE];

  if (cs == pkt_cs)
  {
    // копируем полезные 9 байт
    for (int i = 0; i < 9; i++)
      out[i] = uart_buf[(uart_tail + i) % UART_BUF_SIZE];
    // продвигаем tail на 10
    uart_tail = (uart_tail + 10) % UART_BUF_SIZE;
    return true;
  }
  else
  {
    // ошибка—пропускаем этот байт
    uart_tail = (uart_tail + 1) % UART_BUF_SIZE;
    return false;
  }
}

static const sensorPacket_t simulatedPacket = {
    .raw = { 10,  20,  30,  40,  50,  60,  70,  80,  90 }
};

/**
 * @brief  Simulation task that periodically sends a constant sensor packet.
 * @param  argument  Not used
 */
/*
void CommsTask(void *argument)
{
    (void)argument;
    for (;;)
    {
        // Overwrite the sensor queue with the simulated packet
        xQueueOverwrite(xSensorQueue, &simulatedPacket);

        // Wait 50 ms before sending again
        osDelay(50);
    }
}
*/
/**
 * @brief  Simulation task that periodically sends a constant sensor packet
 *         and indicates activity by toggling LED3 and updating the LCD.
 */
void CommsTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        /* 1) Overwrite the sensor queue with the simulated packet */
        xQueueOverwrite(xSensorQueue, &simulatedPacket);

        /* 2) Toggle LED3 to show this task is running */
        BSP_LED_Toggle(LED3);

        /* 4) Wait 50 ms before sending again */
        osDelay(50);
    }
}

/*
void CommsTask(void *p)
{
  uint8_t packet[9];
  for (;;)
  {
    // ждём хоть один байт
    while ((uart_head + UART_BUF_SIZE - uart_tail) % UART_BUF_SIZE < 1)
      vTaskDelay(1);

    // парсим пакеты
    while (TryParsePacket(packet))
    {
      // сюда придут 9 измерений
      //ProcessIncomingData(packet);
    }
    // ждать до следующего чанка
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
*/

void CommsInit(void)
{
 // MX_USART3_UART_Init();
  HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
//  xTaskCreate(CommsTask, "Comms", 256, NULL, 1, NULL);
}
