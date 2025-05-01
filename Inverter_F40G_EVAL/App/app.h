/* app.h      ← put in Core/Inc/ */
#pragma once
#include <stdint.h>

typedef struct {
    uint8_t raw[9];      // 9 data bytes from UART packet
} sensorPacket_t;

typedef struct {
    float level;         // control level 0.0..1.0 for PWM amplitude
} control_t;


void LEDUITask(void *arg);
void SineGenTask(void *arg);
void ControlTask(void *arg);
void CommsTask(void *arg);
void CommsInit(void);
