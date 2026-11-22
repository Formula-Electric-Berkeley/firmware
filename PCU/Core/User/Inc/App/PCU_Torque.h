/**
 ******************************************************************************
 * @file           : PCU_Torque.h
 * @brief          : APPS plausibility and torque command
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#ifndef PCU_TORQUE_H
#define PCU_TORQUE_H

#include <stdbool.h>
#include <stdint.h>

extern uint32_t last_sample_ms;
extern uint32_t last_plausible_ms;
extern uint32_t last_bse_plausible_ms;
extern bool apps_implausibility;
extern bool brake_implausibility;
extern uint16_t torque_command;

void PCU_Tick(void);

uint16_t PCU_GetTorqueCommand(void);

bool PCU_GetAppsFault(void);

#endif /* PCU_TORQUE_H */
