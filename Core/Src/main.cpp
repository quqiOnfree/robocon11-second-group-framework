/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.cpp
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "bsp_gpio_pin.hpp"
#include "bsp_mutex.hpp"
#include "bsp_semaphore.hpp"
#include "bsp_thread.hpp"
#include "bsp_type_traits.hpp"
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

void SystemClock_Config(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  HAL_Init();
  SystemClock_Config();
  osKernelInitialize();

  __HAL_RCC_GPIOF_CLK_ENABLE();

  // 初始化全局内存资源和互斥量，确保在任何线程使用前都已准备就绪
  gdut::thread_memory_resource::pool_mutex = gdut::mutex{};

  gdut::gpio_pin<gdut::gpio_port::F,
                 GPIO_InitTypeDef{.Pin = GPIO_PIN_9,
                                  .Mode = GPIO_MODE_OUTPUT_PP,
                                  .Pull = GPIO_NOPULL,
                                  .Speed = GPIO_SPEED_FREQ_LOW,
                                  .Alternate = 0}>
      led0;
  gdut::gpio_pin<gdut::gpio_port::F,
                 GPIO_InitTypeDef{.Pin = GPIO_PIN_10,
                                  .Mode = GPIO_MODE_OUTPUT_PP,
                                  .Pull = GPIO_NOPULL,
                                  .Speed = GPIO_SPEED_FREQ_LOW,
                                  .Alternate = 0}>
      led1;

  gdut::thread<4 * 128> main_thread([&led0, &led1]() {
    led0.write(true);
    led1.write(true);
    gdut::binary_semaphore semaphore1{0u};
    gdut::binary_semaphore semaphore2{0u};

    gdut::thread<4 * 128> thread1([&semaphore1, &semaphore2]() {
      for (int i = 0; i < 2500; ++i) {
        semaphore1.release();
        semaphore2.acquire();
      }
    });

    gdut::thread<4 * 128> thread2([&semaphore1, &semaphore2]() {
      for (int i = 0; i < 2500; ++i) {
        semaphore1.acquire();
        semaphore2.release();
      }
    });

    thread1.join();
    thread2.join();

    while (true) {
      osDelay(500);
      led1.toggle();
    }
  });

  osKernelStart();

  while (1) {
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
