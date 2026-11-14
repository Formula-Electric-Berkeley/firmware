/**
 ******************************************************************************
 * @file           : feb_can.cpp
 * @brief          : Subscriber dispatch, TX ring and publisher schedule
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "feb_can.hpp"
#include "feb_can_transport.hpp"

#include "cmsis_os2.h"

#include <cstddef>

namespace feb::can
{
namespace detail
{
namespace
{
constexpr std::size_t kRingSize = 16;
constexpr std::size_t kBusCount = 2;

struct BusState
{
  Handle *handle = nullptr;
  FrameHook hook = nullptr;
  Frame ring[kRingSize];
  std::size_t head = 0;
  std::size_t count = 0;
  Stats stats{};
};

BusState buses[kBusCount];
RxBase *rx_list = nullptr;
TxBase *tx_list = nullptr;

class IrqLock
{
public:
  IrqLock() : primask_(__get_PRIMASK()) { __disable_irq(); }
  ~IrqLock() { __set_PRIMASK(primask_); }
  IrqLock(const IrqLock &) = delete;

private:
  const std::uint32_t primask_;
};

BusState &state_of(Bus bus)
{
  return buses[static_cast<std::size_t>(bus)];
}

BusState *state_of(const Handle &handle)
{
  for (BusState &s : buses)
  {
    if (s.handle == &handle)
    {
      return &s;
    }
  }
  return nullptr;
}

// Caller holds IrqLock.
void drain(BusState &s)
{
  while (s.count > 0 && hw_write(*s.handle, s.ring[s.head]))
  {
    s.head = (s.head + 1) % kRingSize;
    s.count--;
  }
}
} // namespace

RxBase::RxBase(Bus bus, std::uint32_t id, bool extended) : next(rx_list), bus(bus), id(id), extended(extended)
{
  rx_list = this;
}

TxBase::TxBase(std::uint32_t period_ms, std::uint32_t offset_ms) : period_ms(period_ms), due_ms(offset_ms)
{
  if (period_ms != 0)
  {
    next = tx_list;
    tx_list = this;
  }
}

bool send(Bus bus, const Frame &frame)
{
  BusState &s = state_of(bus);
  IrqLock lock;
  if (s.handle == nullptr)
  {
    return false;
  }
  if (s.count == kRingSize) // overwrite the oldest: newer data is worth more
  {
    s.head = (s.head + 1) % kRingSize;
    s.count--;
    s.stats.tx_dropped++;
  }
  s.ring[(s.head + s.count) % kRingSize] = frame;
  s.count++;
  drain(s);
  return true;
}

void on_rx(Handle &handle, const Frame &frame)
{
  BusState *s = state_of(handle);
  if (s == nullptr)
  {
    return;
  }
  const Bus bus = static_cast<Bus>(s - buses);
  if (s->hook != nullptr)
  {
    s->hook(bus, frame);
  }
  for (RxBase *r = rx_list; r != nullptr; r = r->next)
  {
    if (r->bus == bus && r->id == frame.id && r->extended == frame.extended)
    {
      if (r->deliver(frame.data, frame.length))
      {
        s->stats.rx++;
      }
      else
      {
        s->stats.rx_errors++;
      }
      return;
    }
  }
}

void on_tx_done(Handle &handle, std::uint32_t sent, std::uint32_t failed)
{
  if (BusState *s = state_of(handle))
  {
    IrqLock lock;
    s->stats.tx += sent;
    s->stats.tx_failed += failed;
    drain(*s);
  }
}

void on_rx_lost(Handle &handle)
{
  if (BusState *s = state_of(handle))
  {
    s->stats.rx_lost++;
  }
}

void on_bus_off(Handle &handle)
{
  if (BusState *s = state_of(handle))
  {
    s->stats.bus_off++;
  }
}
} // namespace detail

bool start(Bus bus, Handle &handle, FrameHook every_frame)
{
  detail::FilterId ids[detail::kMaxFilterIds];
  std::size_t count = 0;
  for (const detail::RxBase *r = detail::rx_list; every_frame == nullptr && r != nullptr; r = r->next)
  {
    if (r->bus != bus)
    {
      continue;
    }
    if (count == detail::kMaxFilterIds)
    {
      return false;
    }
    ids[count++] = {r->id, r->extended};
  }

  detail::BusState &s = detail::state_of(bus);
  s.handle = &handle;
  s.hook = every_frame;
  if (!detail::hw_start(handle, every_frame != nullptr, ids, count))
  {
    s.handle = nullptr;
    s.hook = nullptr;
    return false;
  }
  return true;
}

Stats stats(Bus bus)
{
  detail::IrqLock lock;
  return detail::state_of(bus).stats;
}

[[noreturn]] void run_publishers()
{
  const std::uint32_t begin = detail::now_ms();
  for (detail::TxBase *t = detail::tx_list; t != nullptr; t = t->next)
  {
    t->due_ms += begin;
  }

  for (;;)
  {
    const std::uint32_t now = detail::now_ms();
    for (detail::TxBase *t = detail::tx_list; t != nullptr; t = t->next)
    {
      if (static_cast<std::int32_t>(now - t->due_ms) < 0)
      {
        continue;
      }
      t->emit();
      t->due_ms += t->period_ms;
      if (static_cast<std::int32_t>(now - t->due_ms) >= 0)
      {
        t->due_ms = now + t->period_ms; // fell behind: skip the missed cycles
      }
    }
    osDelay(1);
  }
}

} // namespace feb::can
