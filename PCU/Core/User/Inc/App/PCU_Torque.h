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

void PCU_Tick(void);

uint16_t PCU_GetTorqueCommand(void);

bool PCU_GetAppsFault(void);

#endif /* PCU_TORQUE_H */
