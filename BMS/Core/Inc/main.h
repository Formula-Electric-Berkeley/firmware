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
#define SOFT_LIGHT_Pin GPIO_PIN_13
#define SOFT_LIGHT_GPIO_Port GPIOC
#define BMS_IND_Pin GPIO_PIN_0
#define BMS_IND_GPIO_Port GPIOC
#define BMS_A_Pin GPIO_PIN_1
#define BMS_A_GPIO_Port GPIOC
#define PC_AIR_Pin GPIO_PIN_2
#define PC_AIR_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_3
#define SPI1_CS_GPIO_Port GPIOC
#define TSMS_ON_IND_Pin GPIO_PIN_0
#define TSMS_ON_IND_GPIO_Port GPIOA
#define IMD_IND_EN_Pin GPIO_PIN_1
#define IMD_IND_EN_GPIO_Port GPIOA
#define IMD_CHECK_Pin GPIO_PIN_4
#define IMD_CHECK_GPIO_Port GPIOA
#define AIR_M_SENSE_Pin GPIO_PIN_4
#define AIR_M_SENSE_GPIO_Port GPIOC
#define AIR_P_SENSE_Pin GPIO_PIN_5
#define AIR_P_SENSE_GPIO_Port GPIOC
#define WAKE1_Pin GPIO_PIN_0
#define WAKE1_GPIO_Port GPIOB
#define INT1_OUT_Pin GPIO_PIN_1
#define INT1_OUT_GPIO_Port GPIOB
#define TSSI_IN_Pin GPIO_PIN_2
#define TSSI_IN_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_6
#define SPI2_CS_GPIO_Port GPIOC
#define WAKE2_OUT_Pin GPIO_PIN_7
#define WAKE2_OUT_GPIO_Port GPIOC
#define INT2_OUT_Pin GPIO_PIN_8
#define INT2_OUT_GPIO_Port GPIOC
#define BMS_ON_IND_Pin GPIO_PIN_8
#define BMS_ON_IND_GPIO_Port GPIOA
#define AIR_ON_IND_Pin GPIO_PIN_9
#define AIR_ON_IND_GPIO_Port GPIOA
#define CAN1_RX_Pin GPIO_PIN_11
#define CAN1_RX_GPIO_Port GPIOA
#define CAN1_TX_Pin GPIO_PIN_12
#define CAN1_TX_GPIO_Port GPIOA
#define IN_ON_IND_Pin GPIO_PIN_10
#define IN_ON_IND_GPIO_Port GPIOC
#define IMD_ON_IND_Pin GPIO_PIN_11
#define IMD_ON_IND_GPIO_Port GPIOC
#define PRE_ON_IND_Pin GPIO_PIN_12
#define PRE_ON_IND_GPIO_Port GPIOC
#define PC_RELAY_Pin GPIO_PIN_2
#define PC_RELAY_GPIO_Port GPIOD
#define BMS_RESET_Pin GPIO_PIN_5
#define BMS_RESET_GPIO_Port GPIOB
#define PG_Pin GPIO_PIN_6
#define PG_GPIO_Port GPIOB
#define ALERT_Pin GPIO_PIN_7
#define ALERT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
