#include "App/DASH_IO.h"
#include "stm32u5xx_hal.h"

static uint32_t end_buzzer_tick = 0;

static IO_States_t state = {.switch_coolant_pump = false,
                            .switch_radiator_fan = false,
                            .switch_accumulator_fans = false,
                            .switch_logging = false,
                            .button_rtd = false,
                            .button_2 = false,
                            .button_3 = false,
                            .button_4 = false,
                            .buzzer_enabled = false};

// MARK: Switches
void FEB_IO_Update_GPIO(void)
{
  state.button_rtd = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
  state.button_2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
  state.button_3 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
  state.button_4 = HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0);

  state.switch_coolant_pump = HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_10);
  state.switch_radiator_fan = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
  state.switch_accumulator_fans = HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5);
  state.switch_logging = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);

  FEB_IO_Update_Buzzer();
}

// MARK: Buzzer
void FEB_IO_Set_Buzzer(bool new_state)
{
  state.buzzer_enabled = new_state;
}

void FEB_IO_Update_Buzzer(void)
{
  FEB_IO_Set_Buzzer(end_buzzer_tick > HAL_GetTick()); // end_buzzer_tick determines if and how long to play the buzzer
}

void FEB_IO_Play_Buzzer(uint32_t duration)
{
  end_buzzer_tick = HAL_GetTick() + duration;
}

IO_States_t FEB_IO_GetLastIOStates(void)
{
  return state;
}
