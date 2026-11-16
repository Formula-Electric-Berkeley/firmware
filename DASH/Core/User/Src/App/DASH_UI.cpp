/**
 ******************************************************************************
 * @file           : DASH_UI.cpp
 * @brief          : LVGL bring-up updating subjects
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "DASH_UI.h"
#include "UI_gen.h"
#include "feb_can.hpp"
#include "feb_can_traits.hpp"
#include "lvgl/core/lv_observer.h"
#include "lvgl_port_display.h"
#include "lvgl_port_touch.h"
#include "UI.h"
#include "main.h"
#include "stm32u5xx_hal_gpio.h"
#include <cstdint>
#include <string>

namespace fc = feb::can;
namespace fm = feb::can::msg;

void DASH_UI_Init()
{
  lv_init();
  lv_tick_set_cb(HAL_GetTick);

  lvgl_display_init();
  lvgl_touchscreen_init();

  UI_init("A:");
  lv_screen_load(screen_feb_create());
}

uint32_t DASH_UI_Update()
{
  lv_subject_set_int(subject_wheel_speed, fc::rx<fm::WssRear>.fresh() ? fc::rx<fm::WssRear>.v().wss_left / 100 : 67);
  // lv_subject_set_int(subject_torque_percent, fc::rx<fm::M192CommandMessage>.fresh()
  //                                                ? fc::rx<fm::M192CommandMessage>.v().vcu_inv_torque_command / 220
  //                                                : 67);
  lv_subject_set_int(subject_torque_percent, fc::rx<fm::M192CommandMessage>.v().vcu_inv_torque_command / 220);

  lv_subject_set_string(subject_funny_text, !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
                                                ? "alex is a very kind gentleman - faris"
                                                : "button not pressed");

  lv_subject_set_int(subject_switches_off,
                     ((uint8_t)!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_10)) + !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) +
                         !HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5) + !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10));
  lv_subject_set_int(subject_switches_on,
                     ((uint8_t)!HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9)) + !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) +
                         !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) + !HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_4));

  return lv_timer_handler();
}
