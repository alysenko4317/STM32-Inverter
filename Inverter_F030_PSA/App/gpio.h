// gpio.h — board‐specific GPIO pin definitions & helpers

#ifndef GPIO_H
#define GPIO_H

#include "stm32f0xx_hal.h"

// Just the LED defines (moved from main.h after CubeMX generation)
#define LED_B_Pin        GPIO_PIN_3
#define LED_B_GPIO_Port  GPIOB
#define LED_A_Pin        GPIO_PIN_4
#define LED_A_GPIO_Port  GPIOB

// Simple inline toggle functions
static inline void LED_A_Toggle(void) {
    HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
}
static inline void LED_B_Toggle(void) {
    HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
}

#endif // GPIO_H
