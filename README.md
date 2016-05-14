## Introduction

This repository contains code for implementing a rhythm master game on STM32f103ZE6 SOC.

## Equipment and Software
* Alientek STM32 project board
* KEIL MDK-ARM IDE
* Standard Firmware Library for STM32F10x

## Desciprtion

The whole project is developed using Standard Firmware Library, and the development is done through manipulating registers directly rather than invoking function defined in library.

The repository contains two folder:

1. **Board** contains the building block for project like buzzer, LCD display, Timer setting, USART configuration, Interrupt configuration and Key setting. The building block facilitates the development process.
2. **User** contains the main C file which is the main entry for the project.

The demo can be found [here](http://v.youku.com/v_show/id_XMTU2OTk0NzIxMg==.html?from=y1.7-1.2).

## Reference
* The definitive guide to the ARM Cortex-M3 2nd Edition
* STM32F10x Reference Manual (RM0008)

