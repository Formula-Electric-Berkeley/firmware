#pragma once

/**
 ******************************************************************************
 * @file           : feb_can_transport.hpp
 * @brief          : Contract between the core and the bxCAN / FDCAN transport
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "feb_can.hpp"

#include <cstddef>

namespace feb::can::detail
{
struct FilterId
{
  std::uint32_t id;
  bool extended;
};

// The largest filter budget of any transport (FDCAN: 56 standard + 16 extended).
inline constexpr std::size_t kMaxFilterIds = 72;

// Implemented by the transport. hw_write runs under the core's IRQ lock.
// accept_all: pass every frame and ignore `ids`. False if the ids do not fit.
bool hw_start(Handle &handle, bool accept_all, const FilterId *ids, std::size_t count);
// False if no hardware TX slot is free; the frame stays queued.
bool hw_write(Handle &handle, const Frame &frame);

// Implemented by the core, called from the transport's interrupt callbacks.
void on_rx(Handle &handle, const Frame &frame);
void on_tx_done(Handle &handle, std::uint32_t sent, std::uint32_t failed);
void on_rx_lost(Handle &handle);
void on_bus_off(Handle &handle);
} // namespace feb::can::detail
