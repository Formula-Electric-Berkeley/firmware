/**
 ******************************************************************************
 * @file           : PCU_Torque.cpp
 * @brief          : Every FreeRTOS task body on PCU
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "PCU_ADC.hpp"
#include "PCU_CAN.h"
#include "PCU_Commands.h"
#include "PCU_Config.hpp"
#include "cmsis_os2.h"
#include "feb_can_tasks.hpp"
#include "feb_uart.h"
#include "stm32f4xx_hal.h"

uint32_t last_sample_ms = 0;
uint32_t last_plausible_ms = 0;
uint32_t last_bse_plausible_ms = 0;
bool apps_implausibility = false;
bool brake_implausibility = false;
uint16_t torque_command = 0;

// HAL_GetTick() returns the time in ms since boot
// Torque.cpp includes tandem code for BSE implausibility check.
// if (PCU_CAN_IsR2D() && !apps_fault) <- implement when BMS is ready

void PCU_Tick()
{
  uint32_t now = HAL_GetTick();
  if ((now - last_sample_ms) < PCU_SAMPLE_PERIOD_MS)
  {
    return;
  }
  last_sample_ms = now;

  bool apps_plausible = validate_torque(apps1, apps2) && apps1.in_range() && apps2.in_range();
  if (apps_plausible)
  {
    last_plausible_ms = now;
  }
  apps_implausibility = (now - last_plausible_ms) > APPS_IMPLAUSIBILITY_TIME_MS;
  if (!apps_implausibility)
  {
    torque_command = ((apps1.get_percentage() + apps2.get_percentage()) * MAX_TORQUE) / 2;
  }

  bool bse_plausible = brake1.in_range() && brake2.in_range();
  if (bse_plausible)
  {
    last_bse_plausible_ms = now;
  }
  brake_implausibility = (now - last_bse_plausible_ms) > BRAKE_IMPLAUSIBILITY_TIME_MS;
  if (brake_implausibility)
  {
    torque_command = 0;
  }
}
