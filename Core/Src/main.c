/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "HTS221.h"
#include "stdbool.h"
#include "LPS22HB.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
hts221_t HTS221;
LPS22HB_t LPS22HB;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int16_t temp;
uint16_t humidity;
int16_t pressure;
int16_t tempa;
int16_t message[32];
uint8_t length;

RTC_TimeTypeDef rtc_time;
RTC_DateTypeDef	rtc_date;

volatile uint8_t RTC_ALARM_FLAG;
volatile uint8_t DATA_READY_FLAG;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void RTC_set_alarm(RTC_TimeTypeDef* RTC_time, uint8_t time_interval_minutes);




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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HTS221_Init(&HTS221, &hi2c1, DEV_ADDR);
  LPS22HB_Init(&LPS22HB, LPS_DEV_ADDR, &hi2c1);

  HTS221_DRDY_Enable(&HTS221, OPEN_DRAIN, ACTIVE_LOW);
  HTS221_set_output_data_rate(&HTS221, Hz_1);
  HTS221_get_data(&HTS221, &temp, &humidity);

  LPS22HB_Set_ODR(&LPS22HB, rate_1Hz);
  LPS22HB_Set_DRDY_Signal(&LPS22HB, DRDY_ActiveLow, DRDY_OpenDrain, 1);
  LPS22HB_GetData(&LPS22HB, &pressure, &tempa);
  HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
  RTC_set_alarm(&rtc_time, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

	  if(RTC_ALARM_FLAG == 0)
	  {
		  RTC_ALARM_FLAG = 1;

		  if(DATA_READY_FLAG == 1)
		  {
			  HTS221_get_data(&HTS221, &temp, &humidity);
			  DATA_READY_FLAG = 0;
		  }

		  if(HAL_GPIO_ReadPin(LPS22HB_DRDY_GPIO_Port, LPS22HB_DRDY_Pin) == GPIO_PIN_RESET)
		  {
			  LPS22HB_GetData(&LPS22HB, &pressure, &tempa);
		  }


		  length = sprintf(&message, "Pressure:%dhPa\r\nTemperature:%dC\r\nHumidity:%d%%\r\n\r\n", pressure, temp, humidity);
		  HAL_UART_Transmit(&huart2, &message, length, 1000);

	  }
	  //HTS221_get_data_OneHot(&HTS221, &temp, &humidity);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == HTS221_DRDY_Pin)
	{
		DATA_READY_FLAG = 1;
	}

}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_ALARM_FLAG = 0;

	RTC_set_alarm(&rtc_time, 1);
}


void RTC_set_alarm(RTC_TimeTypeDef* RTC_time, uint8_t time_interval_minutes)
{
	RTC_AlarmTypeDef sAlarm = {0};

	sAlarm.Alarm = RTC_ALARM_A;
	sAlarm.AlarmTime.Minutes = RTC_time->Minutes + time_interval_minutes;
	sAlarm.AlarmTime.Seconds = RTC_time->Seconds;
	sAlarm.AlarmTime.Hours = RTC_time->Hours;


	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
