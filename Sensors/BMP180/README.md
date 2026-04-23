# STM32 BMP180 Pressure Sensor Test

## Project Overview

This project tests the BMP180 barometric pressure sensor using an STM32 Nucleo-F446RE development board. The BMP180 communicates with the STM32 through I2C and outputs temperature and atmospheric pressure data over UART serial communication.

The purpose of this project is to verify that the BMP180 sensor can be successfully connected, detected, calibrated, and read by the STM32. This is part of the sensor bring-up process for a larger drone flight controller senior design project.

## Hardware Used

- STM32 Nucleo-F446RE
- BMP180 barometric pressure sensor module
- Jumper wires
- USB cable for programming and serial communication

## Software Used

- STM32CubeIDE
- STM32CubeMX configuration through `.ioc` file
- HAL drivers
- Serial terminal inside STM32CubeIDE

## Pin Connections

| BMP180 Pin | STM32 Nucleo-F446RE Pin |
|---|---|
| VCC / VIN | 3.3V |
| GND | GND |
| SCL | PB8 / I2C1_SCL |
| SDA | PB9 / I2C1_SDA |

## Peripheral Configuration

### I2C1

| Setting | Value |
|---|---|
| Peripheral | I2C1 |
| SCL Pin | PB8 |
| SDA Pin | PB9 |
| I2C Speed | 100 kHz |
| Addressing Mode | 7-bit |

### UART2

UART2 is used to print sensor readings to the serial terminal.

| Setting | Value |
|---|---|
| Peripheral | USART2 |
| Baud Rate | 115200 |
| Word Length | 8 bits |
| Stop Bits | 1 |
| Parity | None |
| Flow Control | None |

### LED Indicator

LD2 on the Nucleo board blinks while the program is running. This confirms that the main loop is active.

## BMP180 I2C Address

The BMP180 uses the I2C address:

```c
0x77
