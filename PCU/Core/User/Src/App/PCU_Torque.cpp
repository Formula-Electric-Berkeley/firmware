/**
 ******************************************************************************
 * @file           : PCU_Torque.cpp
 * @brief          : APPS plausibility and torque command
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "App/PCU_Torque.h"
#include "PCU_ADC.hpp"
#include "PCU_CAN.h"

#include "main.h"

namespace
{
uint32_t last_sample_ms = 0;
uint32_t last_plausible_ms = 0;
bool apps_fault = false;
uint16_t torque_command = 0;
} // namespace

void PCU_Tick(void)
{
  uint32_t now = HAL_GetTick(); // gets the time in ms since boot
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
  apps_fault = (now - last_plausible_ms) > APPS_IMPLAUSIBILITY_TIME_MS;

  torque_command = 0;
  if (PCU_CAN_IsR2D() && !apps_fault)
  {
    torque_command = apps1.get_torque();
  }
}

uint16_t PCU_GetTorqueCommand(void)
{
  return torque_command;
}

bool PCU_GetAppsFault(void)
{
  return apps_fault;
}
