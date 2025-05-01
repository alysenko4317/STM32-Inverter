# Project: Inverter Control Firmware

## Overview
This project implements control firmware for a power inverter on an STM32F4 development board. It reads measurement data over UART, processes it in a control loop, and generates PWM signals to drive MOSFET gates for primary and secondary stages, as well as a sine-wave shaper. The architecture is portable and can be retargeted to an STM32F030-based production module.

## Features
- **Comms**: Receives measurement packets (10 bytes) over UART with XOR checksum.  
- **Sensor Processing**: Parses packets into sensor data structures.  
- **Control Loop**: Computes PWM duty cycles based on feedback.  
- **Sine Generation**: Pre-computed sine table + smooth ramp start/stop.  
- **LED & UI**: Heartbeat LED toggle and optional LCD updates (on F4).  
- **RTOS**: FreeRTOS-based task scheduling on F4 target.  
- **Bare-metal Port**: Single super-loop variant for STM32F030 with no RTOS.

## Repository Structure
```plaintext
YourProject/
├── .ioc                        # CubeMX configuration file
├── Core/
│   ├── Inc/                    # Application headers
│   │   ├── FreeRTOSConfig.h
│   │   ├── main.h
│   │   ├── comms.h
│   │   ├── sensor.h
│   │   ├── sinegen.h
│   │   └── led_ui.h
│   └── Src/                    # Application sources
│       ├── main.c
│       ├── freertos.c
│       ├── comms.c
│       ├── sinegen.c
│       └── led_ui.c
├── Drivers/
│   ├── STM32F4xx_HAL_Driver/   # HAL drivers
│   ├── STM32F030_HAL_Driver/   # HAL drivers
│   └── BSP/
│       └── STM324xG_eval/      # BSP for F4 eval board
│       └── STM32F030/          # BSP for F0 production board
├── Middlewares/
│   └── FreeRTOS/               # FreeRTOS kernel and config
├── LinkerScripts/
│   ├── STM32F407IGHX_FLASH.ld
│   └── STM32F407IGHX_RAM.ld
├── Target_F030/                # F030 port (bare-metal)
│   ├── Core/Inc/
│   ├── Core/Src/main_f0.c
├── Makefile                    # Multi-target build script
└── .gitignore                  # Ignored artifacts
```

## Module Description
- **Core/**: Target-agnostic application logic (packet parsing, control, sine gen, LED).  
- **Drivers/STM32F4xx_HAL_Driver**: HAL support for F4 (ADC, TIM, UART, GPIO).  
- **Drivers/BSP/STM324xg_eval**: LCD, touch, LED initialization for evaluation board.  
- **Middlewares/FreeRTOS**: RTOS scheduler and API for multitasking on F4.  
- **Release_F0/**: Ported bare-metal files for STM32F030 (no RTOS, minimal HAL).  
- **LinkerScripts/**: Memory layouts for both F4 and F030 targets.  
- **Makefile**: Builds either F4-demo (with RTOS) or F0-release (bare-metal) via `make TARGET=F4` or `make TARGET=F0`.

## Building and Running
```sh
# F4 Demo (with FreeRTOS & LCD)
make clean all TARGET=F4
# Flash the output via ST-LINK or CubeProgrammer.
# Observe LCD and LED heartbeat.

# F0 Release (bare-metal, no LCD)
make clean all TARGET=F0
# Programs 32 KB flash on STM32F030.
# Uses super-loop for UART parsing, control, PWM, LED.
```

## Extensibility
- **Add peripheral tasks**: network, USB, SD by creating new tasks/modules.
- **Retarget**: implement `bsp_fX.c` for any other STM32.
- **Simulation**: unit-test Core logic on PC by mocking HAL interfaces.

