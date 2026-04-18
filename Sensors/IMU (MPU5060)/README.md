# STM32 IMU Test Project

## Purpose

This project tests IMU communication on the STM32 Nucleo board for the senior design drone flight controller project. It verifies that the microcontroller can successfully communicate with an IMU sensor and retrieve real-time motion data.

---

## Hardware

* STM32 Nucleo-F446RE
* MPU6050
* USB connection to PC
* PuTTY (or similar serial terminal)

---

## Wiring

* VCC → 3.3V
* GND → GND
* SDA → PB9 (I2C1 SDA)
* SCL → PB8 (I2C1 SCL)
* AD0 → GND

---

## Peripherals Used

* **I2C1** for IMU communication
* **USART2** for serial output at 116200 baud

---

## What the Code Does

1. Initializes STM32 peripherals (GPIO, I2C, UART)
2. Reads the IMU `WHO_AM_I` register to confirm communication
3. Wakes the IMU from sleep mode
4. Reads accelerometer, gyroscope, and temperature data
5. Converts raw sensor values into usable units
6. Sends formatted data to the serial terminal

---

## Variables Explained

### Accelerometer (g)

* **Ax, Ay, Az** → Acceleration along X, Y, Z axes
* Measured in **g (gravity units)**
* When stationary, one axis should read ~±1.0 due to gravity

### Gyroscope (deg/s)

* **Gx, Gy, Gz** → Angular velocity around X, Y, Z axes
* Measured in **degrees per second**
* Indicates how fast the sensor is rotating

### Temperature

* Internal sensor temperature in **°C**

---

## Expected Output

The serial terminal should display:

Starting MPU6050 test...
WHO_AM_I read status = 0, value = 0x70
MPU6500 or clone detected
Ax: ... Ay: ... Az: ... | Gx: ... Gy: ... Gz: ... | T: ...

Values should update continuously and change when the sensor is moved.

---

## Current Result

The connected IMU returns `WHO_AM_I = 0x70`, indicating it is likely an MPU6500-compatible or clone device rather than a true MPU6050. Communication and data acquisition are successful.

---

## How to Run

1. Open project in STM32CubeIDE
2. Build and flash code to Nucleo board
3. Open PuTTY (COM5 port, 116200 baud)
4. Press reset on the board
5. Observe real-time IMU data
