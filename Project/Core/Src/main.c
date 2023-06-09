/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "bmp280.h"
#include "sgp30.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BMP280_HandleTypedef bmp280;
float pressure, temperature, humidity;
uint16_t sizeValue;
uint8_t dataValue[256];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  float temperatureValue = 0;
  float pressureValue = 0;
  float formaldehydeValue = 0;
  uint8_t standardTemperatureValue = 30;
  float standardformaldehydeValue = 0.08;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  bmp280_init_default_params(&bmp280.params);
  bmp280.addr = BMP280_I2C_ADDRESS_0;
  bmp280.i2c = &hi2c1;
  while (!bmp280_init(&bmp280, &bmp280.params))
  {
    sizeValue = sprintf((char *)dataValue, "BMP280 initialization failed\n");
    HAL_Delay(2000);
  }
  bool bme280p = bmp280.id == BME280_CHIP_ID;
  sizeValue = sprintf((char *)dataValue, "BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

  SGP30_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (HAL_GPIO_ReadPin(HCSR505_IO_GPIO_Port, HCSR505_IO_Pin) == GPIO_PIN_SET)
    {
      while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
      {
        sizeValue = sprintf((char *)dataValue, "Temperature/pressure reading failed!\n");
        HAL_Delay(2000);
      }
      sizeValue = sprintf((char *)dataValue, "Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
      if (bme280p)
      {
        sizeValue = sprintf((char *)dataValue, ", Humidity: %.2f\n", humidity);
        HAL_UART_Transmit(&huart1, dataValue, sizeValue, 1000);
      }
      else
      {
        sizeValue = sprintf((char *)dataValue, "\n");
      }

      temperatureValue = temperature;
      pressureValue = pressure / 1000;

      formaldehydeValue = SGP30_GetCO2andVOC() / 1000;

      printf("*-----*-----*-----*-----*-----*\n");
      printf("Current Temperature: %.2f C\n", temperatureValue);
      printf("Current Pressure: %.3f kPa\n", pressureValue);
      printf("Current Formaldehyde: %.2f ppm\n", formaldehydeValue);
      printf("*-----*-----*-----*-----*-----*\n");

      if (temperatureValue > standardTemperatureValue || formaldehydeValue > standardformaldehydeValue)
      {
        HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_SET);
        HAL_Delay(100);
        HAL_GPIO_TogglePin(LED_IO_GPIO_Port, LED_IO_Pin);
      }
    }

    if (HAL_GPIO_ReadPin(Key_1_GPIO_Port, Key_1_Pin) == GPIO_PIN_RESET)
    {
      while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
      {
        sizeValue = sprintf((char *)dataValue, "Temperature/pressure reading failed!\n");
        HAL_Delay(2000);
      }
      sizeValue = sprintf((char *)dataValue, "Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
      if (bme280p)
      {
        sizeValue = sprintf((char *)dataValue, ", Humidity: %.2f\n", humidity);
        HAL_UART_Transmit(&huart1, dataValue, sizeValue, 1000);
      }
      else
      {
        sizeValue = sprintf((char *)dataValue, "\n");
      }

      temperatureValue = temperature;
      pressureValue = pressure / 1000;

      formaldehydeValue = SGP30_GetCO2andVOC() / 1000;

      printf("*-----*-----*-----*-----*-----*\n");
      printf("Current Temperature: %.2f C\n", temperatureValue);
      printf("Current Pressure: %.3f kPa\n", pressureValue);
      printf("Current Formaldehyde: %.2f ppm\n", formaldehydeValue);
      printf("*-----*-----*-----*-----*-----*\n");

      if (temperatureValue > standardTemperatureValue || formaldehydeValue > standardformaldehydeValue)
      {
        HAL_GPIO_WritePin(LED_IO_GPIO_Port, LED_IO_Pin, GPIO_PIN_SET);
        HAL_Delay(100);
        HAL_GPIO_TogglePin(LED_IO_GPIO_Port, LED_IO_Pin);
      }
    }

    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
