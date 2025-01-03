/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
* @attention
*
* Copyright (c) 2024 STMicroelectronics.
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
#include "icache.h"
#include "usart.h"
#include "memorymap.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "icm42688.h"
#include "m24m02.h"
#include "w25qxx.h"
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
uint8_t rx_data = 0;
uint8_t button_val = 0;
uint8_t hello1[] = "Hello M24M02!";
uint8_t hello2[] = "Hello W25QXX!";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//重定向printf函数到串口
int fputc(int ch, FILE * f)
{
  HAL_UART_Transmit(&hlpuart1, (uint8_t*)&ch, 1, 10);
  return ch;
}

//重定向scanf函数到串口
int fgetc(FILE * F)
{
  uint8_t ch;
  HAL_UART_Receive(&hlpuart1, (uint8_t*)&ch, 1, 10);
  return ch;
}

// 测试键盘功能
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    if(rx_data >= 0 && rx_data < 0x0c){
      printf("You press button %d.\r\n", rx_data);
      button_val = rx_data;
    }else if(rx_data >= 0x80 && rx_data < 0x8c){
      printf("You release button %d.\r\n", rx_data - 0x80);
    }
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_data, 1);
  }
}

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
  MX_ICACHE_Init();
  MX_LPUART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  printf("Test demo\r\n");
  printf("Press button 1 to test led.\r\n");
  printf("Press button 2 to test beep.\r\n");
  printf("Press button 3 to test icm42688.\r\n");
  printf("Press button 4 to test m24m02.\r\n");
  printf("Press button 5 to test w25qxx.\r\n");
  icm42688_detect();
  m24m02_detect();
  w25q_init();
  uint16_t max_block = w25q_detect();
  icm42688_cfg_inv inv = icm42688_default_init();
  icm42688_raw_val val = {0};
  HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_data, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch(button_val)
    {
    case 1:
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 1);
      HAL_GPIO_TogglePin(RED_GPIO_Port, RED_Pin);
      HAL_Delay(300);
      HAL_GPIO_TogglePin(RED_GPIO_Port, RED_Pin);
      HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
      HAL_Delay(300);
      HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
      HAL_GPIO_TogglePin(BLUE_GPIO_Port, BLUE_Pin);
      HAL_Delay(300);
      HAL_GPIO_TogglePin(BLUE_GPIO_Port, BLUE_Pin);
      break;
    case 2:
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 1);
      HAL_Delay(900);
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 0);
      HAL_Delay(100);
      break;
    case 3:
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 1);
      HAL_Delay(600);
      val = icm42688_get_data(inv);
      printf("Read from ICM42688:\r\n");
      printf("Temperature: %.2f\r\n", val.temperature);
      printf("Acc: %.2f, %.2f, %.2f\r\n", val.acc[0], val.acc[1], val.acc[2]);
      printf("Gyro: %.2f, %.2f, %.2f\r\n", val.gyro[0], val.gyro[1], val.gyro[2]);
      break;
    case 4:
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 1);
      HAL_Delay(1000);
      uint8_t e2prom[20] = {0};
      uint16_t e2prom_addr = 0x0001;
      printf("Write \"%s\" to M24M02.\r\n", hello1);
      m24m02_mem_write(e2prom_addr, hello1, strlen(hello1) + 1);
      HAL_Delay(100);
      m24m02_mem_read(e2prom_addr, e2prom, strlen(hello1) + 1);
      printf("Read from M24M02:\r\n");
      printf("%s\r\n", e2prom);
      break;
    case 5:
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, 1);
      HAL_Delay(1000);
      uint8_t flash[20] = {0};
      uint32_t flash_addr = w25q_block2sector_addr(0, 0);
      uint16_t len2 = strlen(hello2);
      printf("Write \"%s\" to W25QXX.\r\n", hello2);
      w25q_write_by_sector(flash_addr, hello2, strlen(hello2) + 1);
      w25q_read_bytes(flash_addr, flash, strlen(hello2) + 1);
      printf("Read from W25QXX:\r\n");
      printf("%s\r\n", flash);
      break;
    }
    
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 62;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
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
