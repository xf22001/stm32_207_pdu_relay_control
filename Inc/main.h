/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f2xx_hal.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define kg9_Pin GPIO_PIN_0
#define kg9_GPIO_Port GPIOA
#define kg10_Pin GPIO_PIN_1
#define kg10_GPIO_Port GPIOA
#define kg11_Pin GPIO_PIN_2
#define kg11_GPIO_Port GPIOA
#define kg12_Pin GPIO_PIN_3
#define kg12_GPIO_Port GPIOA
#define d8_Pin GPIO_PIN_4
#define d8_GPIO_Port GPIOA
#define d4_Pin GPIO_PIN_5
#define d4_GPIO_Port GPIOA
#define d2_Pin GPIO_PIN_6
#define d2_GPIO_Port GPIOA
#define d1_Pin GPIO_PIN_7
#define d1_GPIO_Port GPIOA
#define led_fault_Pin GPIO_PIN_4
#define led_fault_GPIO_Port GPIOC
#define led_can_Pin GPIO_PIN_5
#define led_can_GPIO_Port GPIOC
#define set2_Pin GPIO_PIN_7
#define set2_GPIO_Port GPIOE
#define set1_Pin GPIO_PIN_8
#define set1_GPIO_Port GPIOE
#define fb9_Pin GPIO_PIN_8
#define fb9_GPIO_Port GPIOD
#define fb10_Pin GPIO_PIN_9
#define fb10_GPIO_Port GPIOD
#define fb11_Pin GPIO_PIN_10
#define fb11_GPIO_Port GPIOD
#define fb12_Pin GPIO_PIN_11
#define fb12_GPIO_Port GPIOD
#define kg5_Pin GPIO_PIN_12
#define kg5_GPIO_Port GPIOD
#define kg6_Pin GPIO_PIN_13
#define kg6_GPIO_Port GPIOD
#define kg7_Pin GPIO_PIN_14
#define kg7_GPIO_Port GPIOD
#define kg8_Pin GPIO_PIN_15
#define kg8_GPIO_Port GPIOD
#define kg4_Pin GPIO_PIN_6
#define kg4_GPIO_Port GPIOC
#define kg3_Pin GPIO_PIN_7
#define kg3_GPIO_Port GPIOC
#define kg2_Pin GPIO_PIN_8
#define kg2_GPIO_Port GPIOC
#define kg1_Pin GPIO_PIN_9
#define kg1_GPIO_Port GPIOC
#define fb1_Pin GPIO_PIN_0
#define fb1_GPIO_Port GPIOD
#define fb2_Pin GPIO_PIN_1
#define fb2_GPIO_Port GPIOD
#define fb3_Pin GPIO_PIN_2
#define fb3_GPIO_Port GPIOD
#define fb4_Pin GPIO_PIN_3
#define fb4_GPIO_Port GPIOD
#define fb5_Pin GPIO_PIN_4
#define fb5_GPIO_Port GPIOD
#define fb6_Pin GPIO_PIN_5
#define fb6_GPIO_Port GPIOD
#define fb7_Pin GPIO_PIN_6
#define fb7_GPIO_Port GPIOD
#define fb8_Pin GPIO_PIN_7
#define fb8_GPIO_Port GPIOD
#define usart_con_Pin GPIO_PIN_5
#define usart_con_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
