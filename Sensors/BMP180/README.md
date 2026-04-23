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
- STM32CubeMX configuration through the `.ioc` file
- STM32 HAL drivers
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
```

For STM32 HAL functions, the address is shifted left by one bit:

```c
#define BMP180_ADDR (0x77 << 1)
```

## Program Behavior

When the program starts, it:

1. Initializes the STM32 HAL library.
2. Configures the system clock.
3. Initializes GPIO, I2C1, and USART2.
4. Checks if the BMP180 responds at address `0x77`.
5. Reads the BMP180 calibration coefficients.
6. Continuously reads temperature and pressure data.
7. Prints the sensor values over UART.
8. Toggles LD2 every 500 ms.

## Example Serial Output

```text
Starting BMP180 test...
BMP180 device ready status = 0
BMP180 detected at address 0x77
BMP180 calibration read successful
AC1=7321 AC2=-1037 AC3=-14725 AC4=32741 AC5=24873 AC6=19718
Temp: 24.30 C | Pressure: 101325 Pa | 1013.25 hPa
Temp: 24.31 C | Pressure: 101326 Pa | 1013.26 hPa
Temp: 24.31 C | Pressure: 101324 Pa | 1013.24 hPa
```

The exact values will vary depending on room temperature, altitude, and local atmospheric pressure.

## Units

| Value | Unit | Meaning |
|---|---|---|
| Temperature | °C | Sensor temperature in degrees Celsius |
| Pressure | Pa | Atmospheric pressure in Pascals |
| Pressure | hPa | Atmospheric pressure in hectopascals |

### Pressure Unit Conversion

```text
1 hPa = 100 Pa
```

Example:

```text
101325 Pa = 1013.25 hPa
```

Weather reports commonly use hPa because the values are easier to read. For a flight controller, pressure is useful because air pressure decreases as altitude increases.

## Notes

If the serial terminal prints text but no float numbers, such as:

```text
Temp:  C | Pressure:  Pa | hPa
```

then float printing is not enabled in STM32CubeIDE.

To enable float printing:

```text
Project Properties
→ C/C++ Build
→ Settings
→ Tool Settings
→ MCU Settings
→ Use float with printf from newlib-nano
```

Then clean, rebuild, and flash the project again.

If float printing is still an issue, the code can be changed to print integer-scaled values instead of using `%.2f`.

## Troubleshooting

### No I2C response from BMP180

Check the following:

- BMP180 VCC is connected to 3.3V.
- BMP180 GND is connected to STM32 GND.
- BMP180 SCL is connected to PB8.
- BMP180 SDA is connected to PB9.
- I2C1 is enabled in CubeMX.
- PB8 is configured as I2C1_SCL.
- PB9 is configured as I2C1_SDA.
- Wires are short and firmly connected.
- The BMP180 module is connected directly or through a clean breadboard setup.
