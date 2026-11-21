/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SOFT_LIGHT_Pin|BMS_IND_Pin|BMS_A_Pin|PC_AIR_Pin
                          |SPI1_CS_Pin|SPI2_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TSSI_IN_GPIO_Port, TSSI_IN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PC_RELAY_GPIO_Port, PC_RELAY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SOFT_LIGHT_Pin BMS_IND_Pin BMS_A_Pin PC_AIR_Pin
                           SPI1_CS_Pin SPI2_CS_Pin */
  GPIO_InitStruct.Pin = SOFT_LIGHT_Pin|BMS_IND_Pin|BMS_A_Pin|PC_AIR_Pin
                          |SPI1_CS_Pin|SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TSMS_ON_IND_Pin IMD_IND_EN_Pin IMD_CHECK_Pin BMS_ON_IND_Pin
                           AIR_ON_IND_Pin */
  GPIO_InitStruct.Pin = TSMS_ON_IND_Pin|IMD_IND_EN_Pin|IMD_CHECK_Pin|BMS_ON_IND_Pin
                          |AIR_ON_IND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : AIR_M_SENSE_Pin AIR_P_SENSE_Pin WAKE2_OUT_Pin INT2_OUT_Pin
                           IN_ON_IND_Pin IMD_ON_IND_Pin PRE_ON_IND_Pin */
  GPIO_InitStruct.Pin = AIR_M_SENSE_Pin|AIR_P_SENSE_Pin|WAKE2_OUT_Pin|INT2_OUT_Pin
                          |IN_ON_IND_Pin|IMD_ON_IND_Pin|PRE_ON_IND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : WAKE1_Pin INT1_OUT_Pin PG_Pin ALERT_Pin */
  GPIO_InitStruct.Pin = WAKE1_Pin|INT1_OUT_Pin|PG_Pin|ALERT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TSSI_IN_Pin */
  GPIO_InitStruct.Pin = TSSI_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TSSI_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC_RELAY_Pin */
  GPIO_InitStruct.Pin = PC_RELAY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PC_RELAY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BMS_RESET_Pin */
  GPIO_InitStruct.Pin = BMS_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BMS_RESET_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
