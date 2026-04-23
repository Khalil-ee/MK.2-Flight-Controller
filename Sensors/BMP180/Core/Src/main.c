/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : BMP180 pressure/temperature test with LD2 blink
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

#define BMP180_ADDR     (0x77 << 1)

// BMP180 registers
#define BMP180_REG_CALIB_START  0xAA
#define BMP180_REG_CONTROL      0xF4
#define BMP180_REG_RESULT       0xF6
#define BMP180_CMD_TEMP         0x2E
#define BMP180_CMD_PRESSURE     0x34

// Oversampling setting: 0, 1, 2, or 3
#define BMP180_OSS 0

// Calibration coefficients
int16_t AC1, AC2, AC3;
uint16_t AC4, AC5, AC6;
int16_t B1, B2, MB, MC, MD;

int32_t B5;

float temperature_C;
float pressure_Pa;
float pressure_hPa;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
void debug_print(char *msg);
uint8_t BMP180_ReadCalibration(void);
uint8_t BMP180_ReadRawTemperature(int32_t *UT);
uint8_t BMP180_ReadRawPressure(int32_t *UP);
void BMP180_Calculate(int32_t UT, int32_t UP, float *temp_C, float *press_Pa);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void debug_print(char *msg)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

uint8_t BMP180_ReadU16(uint8_t reg, uint16_t *value)
{
    uint8_t data[2];

    if (HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, reg, 1, data, 2, 100) != HAL_OK)
    {
        return 0;
    }

    *value = (uint16_t)((data[0] << 8) | data[1]);
    return 1;
}

uint8_t BMP180_ReadS16(uint8_t reg, int16_t *value)
{
    uint16_t temp;

    if (!BMP180_ReadU16(reg, &temp))
    {
        return 0;
    }

    *value = (int16_t)temp;
    return 1;
}

uint8_t BMP180_ReadCalibration(void)
{
    if (!BMP180_ReadS16(0xAA, &AC1)) return 0;
    if (!BMP180_ReadS16(0xAC, &AC2)) return 0;
    if (!BMP180_ReadS16(0xAE, &AC3)) return 0;
    if (!BMP180_ReadU16(0xB0, &AC4)) return 0;
    if (!BMP180_ReadU16(0xB2, &AC5)) return 0;
    if (!BMP180_ReadU16(0xB4, &AC6)) return 0;
    if (!BMP180_ReadS16(0xB6, &B1))  return 0;
    if (!BMP180_ReadS16(0xB8, &B2))  return 0;
    if (!BMP180_ReadS16(0xBA, &MB))  return 0;
    if (!BMP180_ReadS16(0xBC, &MC))  return 0;
    if (!BMP180_ReadS16(0xBE, &MD))  return 0;

    return 1;
}

uint8_t BMP180_ReadRawTemperature(int32_t *UT)
{
    uint8_t cmd = BMP180_CMD_TEMP;
    uint8_t data[2];

    if (HAL_I2C_Mem_Write(&hi2c1, BMP180_ADDR, BMP180_REG_CONTROL, 1, &cmd, 1, 100) != HAL_OK)
    {
        return 0;
    }

    HAL_Delay(5);

    if (HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, BMP180_REG_RESULT, 1, data, 2, 100) != HAL_OK)
    {
        return 0;
    }

    *UT = (int32_t)((data[0] << 8) | data[1]);
    return 1;
}

uint8_t BMP180_ReadRawPressure(int32_t *UP)
{
    uint8_t cmd = BMP180_CMD_PRESSURE + (BMP180_OSS << 6);
    uint8_t data[3];

    if (HAL_I2C_Mem_Write(&hi2c1, BMP180_ADDR, BMP180_REG_CONTROL, 1, &cmd, 1, 100) != HAL_OK)
    {
        return 0;
    }

#if BMP180_OSS == 0
    HAL_Delay(5);
#elif BMP180_OSS == 1
    HAL_Delay(8);
#elif BMP180_OSS == 2
    HAL_Delay(14);
#else
    HAL_Delay(26);
#endif

    if (HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, BMP180_REG_RESULT, 1, data, 3, 100) != HAL_OK)
    {
        return 0;
    }

    *UP = (((int32_t)data[0] << 16) | ((int32_t)data[1] << 8) | data[2]) >> (8 - BMP180_OSS);
    return 1;
}

void BMP180_Calculate(int32_t UT, int32_t UP, float *temp_C, float *press_Pa)
{
    int32_t X1, X2, X3;
    int32_t B3, B6;
    uint32_t B4, B7;
    int32_t p;

    // Temperature calculation
    X1 = ((UT - (int32_t)AC6) * (int32_t)AC5) >> 15;
    X2 = ((int32_t)MC << 11) / (X1 + MD);
    B5 = X1 + X2;

    int32_t temp_x10 = (B5 + 8) >> 4;
    *temp_C = temp_x10 / 10.0f;

    // Pressure calculation
    B6 = B5 - 4000;

    X1 = ((int32_t)B2 * ((B6 * B6) >> 12)) >> 11;
    X2 = ((int32_t)AC2 * B6) >> 11;
    X3 = X1 + X2;

    B3 = ((((int32_t)AC1 * 4 + X3) << BMP180_OSS) + 2) >> 2;

    X1 = ((int32_t)AC3 * B6) >> 13;
    X2 = ((int32_t)B1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;

    B4 = ((uint32_t)AC4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)(50000 >> BMP180_OSS);

    if (B7 < 0x80000000)
    {
        p = (B7 * 2) / B4;
    }
    else
    {
        p = (B7 / B4) * 2;
    }

    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

    p = p + ((X1 + X2 + 3791) >> 4);

    *press_Pa = (float)p;
}

/* USER CODE END 0 */

int main(void)
{
    HAL_StatusTypeDef status;
    char buffer[150];

    int32_t UT = 0;
    int32_t UP = 0;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */

    debug_print("Starting BMP180 test...\r\n");

    // Check if BMP180 responds at address 0x77
    status = HAL_I2C_IsDeviceReady(&hi2c1, BMP180_ADDR, 3, 100);

    sprintf(buffer, "BMP180 device ready status = %d\r\n", status);
    debug_print(buffer);

    if (status == HAL_OK)
    {
        debug_print("BMP180 detected at address 0x77\r\n");
    }
    else
    {
        debug_print("No I2C response from BMP180 at 0x77\r\n");
    }

    // Read calibration data
    if (BMP180_ReadCalibration())
    {
        debug_print("BMP180 calibration read successful\r\n");

        sprintf(buffer, "AC1=%d AC2=%d AC3=%d AC4=%u AC5=%u AC6=%u\r\n",
                AC1, AC2, AC3, AC4, AC5, AC6);
        debug_print(buffer);
    }
    else
    {
        debug_print("BMP180 calibration read failed\r\n");
    }

    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */

        if (BMP180_ReadRawTemperature(&UT) && BMP180_ReadRawPressure(&UP))
        {
            BMP180_Calculate(UT, UP, &temperature_C, &pressure_Pa);
            pressure_hPa = pressure_Pa / 100.0f;

            sprintf(buffer,
                    "Temp: %.2f C | Pressure: %.2f Pa | %.2f hPa\r\n",
                    temperature_C, pressure_Pa, pressure_hPa);
            debug_print(buffer);
        }
        else
        {
            debug_print("BMP180 sensor data read failed\r\n");
        }

        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(500);

        /* USER CODE END 3 */
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
