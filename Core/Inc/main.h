/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define ENABLE_DEBUG_PRINTF 1

/* Debug output macros */
#if ENABLE_DEBUG_PRINTF >= 1
    #define DEBUG_PRINTF(...) UART_Printf( __VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

void UART_Printf(UART_HandleTypeDef *huart, const char *format, ...);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define ST4_EAST_Pin GPIO_PIN_12
#define ST4_EAST_GPIO_Port GPIOB
#define ST4_NORTH_Pin GPIO_PIN_13
#define ST4_NORTH_GPIO_Port GPIOB
#define ST4_SOUTH_Pin GPIO_PIN_14
#define ST4_SOUTH_GPIO_Port GPIOB
#define ST4_WEST_Pin GPIO_PIN_15
#define ST4_WEST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define ST4_PORT                GPIOB

// UART Defines for easier usage
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
#define UART_DEBUG &huart1  // UART3 for debug output
#define UART_OUT &huart2    // UART2 for normal output
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
