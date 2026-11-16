/**
 ******************************************************************************
 * @file           : DASH_CAN_Publish.cpp
 * @brief          : Everything DASH transmits
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "feb_can.hpp"
#include "feb_can_traits.hpp"
#include "App/DASH_IO.h"

namespace fc = feb::can;
namespace fm = feb::can::msg;

namespace
{
bool fill_dash_state(feb_can_dash_state_t &m)
{
  auto state = FEB_IO_GetLastIOStates();

  m.button1 = state.button_rtd;
  m.button2 = state.button_2;
  m.button3 = state.button_3;
  m.button4 = state.button_4;
  m.switch1 = state.switch_accumulator_fans;
  m.switch2 = state.switch_coolant_pump;
  m.switch3 = state.switch_logging;
  m.switch4 = state.switch_logging;
  m.buzzer = m.buzzer;
  m.ready_to_drive = m.ready_to_drive;

  return true;
}

bool fill_dash_heartbeat(feb_can_dash_heartbeat_t &m)
{
  (void)m;
  return true;
}

fc::Publisher<fm::DashState> state_tx{fill_dash_state};
fc::Publisher<fm::DashHeartbeat> heartbeat_tx{fill_dash_heartbeat};

} // namespace
