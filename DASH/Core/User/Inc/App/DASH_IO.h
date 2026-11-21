#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Modular handlers */
void FEB_IO_Update_GPIO(void);
void FEB_IO_Update_Buzzer(void);
void FEB_IO_Play_Buzzer(uint32_t duration);

typedef struct
{
  // Switch states
  bool switch_coolant_pump;
  bool switch_radiator_fan;
  bool switch_accumulator_fans;
  bool switch_logging;

  // Button states
  bool button_rtd;
  bool button_2;
  bool button_3;
  bool button_4;

  // Buzzer state
  bool buzzer_enabled;
} IO_States_t;

IO_States_t FEB_IO_GetLastIOStates(void);

void FEB_IO_Set_Buzzer(bool new_state);
