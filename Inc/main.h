/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

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
#define nDTCT_Pin GPIO_PIN_13
#define nDTCT_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_0
#define LED3_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOA
#define LED0_Pin GPIO_PIN_3
#define LED0_GPIO_Port GPIOA
#define nOUT_EN0_Pin GPIO_PIN_4
#define nOUT_EN0_GPIO_Port GPIOA
#define ICART_Pin GPIO_PIN_7
#define ICART_GPIO_Port GPIOA
#define GP2_Pin GPIO_PIN_4
#define GP2_GPIO_Port GPIOC
#define GP3_Pin GPIO_PIN_5
#define GP3_GPIO_Port GPIOC
#define GP0_Pin GPIO_PIN_0
#define GP0_GPIO_Port GPIOB
#define GP1_Pin GPIO_PIN_1
#define GP1_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define nOUT_EN1_Pin GPIO_PIN_11
#define nOUT_EN1_GPIO_Port GPIOF
#define GP5_Pin GPIO_PIN_12
#define GP5_GPIO_Port GPIOB
#define GP4_Pin GPIO_PIN_13
#define GP4_GPIO_Port GPIOB
#define GP7_Pin GPIO_PIN_14
#define GP7_GPIO_Port GPIOB
#define GP6_Pin GPIO_PIN_15
#define GP6_GPIO_Port GPIOB
#define SEL0_Pin GPIO_PIN_7
#define SEL0_GPIO_Port GPIOG
#define SEL1_Pin GPIO_PIN_8
#define SEL1_GPIO_Port GPIOG
#define GP8_Pin GPIO_PIN_6
#define GP8_GPIO_Port GPIOC
#define VSEL1_Pin GPIO_PIN_7
#define VSEL1_GPIO_Port GPIOC
#define VSEL0_Pin GPIO_PIN_8
#define VSEL0_GPIO_Port GPIOA
#define nRD_Pin GPIO_PIN_4
#define nRD_GPIO_Port GPIOD
#define nWR_Pin GPIO_PIN_5
#define nWR_GPIO_Port GPIOD
#define nCE0_Pin GPIO_PIN_7
#define nCE0_GPIO_Port GPIOD
#define nCE1_Pin GPIO_PIN_9
#define nCE1_GPIO_Port GPIOG
#define nCE2_Pin GPIO_PIN_10
#define nCE2_GPIO_Port GPIOG
#define nCE3_Pin GPIO_PIN_12
#define nCE3_GPIO_Port GPIOG
#define nSDCD_Pin GPIO_PIN_15
#define nSDCD_GPIO_Port GPIOG
#define SCL1_Pin GPIO_PIN_6
#define SCL1_GPIO_Port GPIOB
#define SDA1_Pin GPIO_PIN_7
#define SDA1_GPIO_Port GPIOB
#define BOOT_EN_Pin GPIO_PIN_8
#define BOOT_EN_GPIO_Port GPIOB
#define nSDWP_Pin GPIO_PIN_9
#define nSDWP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
