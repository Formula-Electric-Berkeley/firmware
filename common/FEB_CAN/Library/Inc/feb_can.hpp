#pragma once

/**
 ******************************************************************************
 * @file           : feb_can.hpp
 * @brief          : Typed CAN publish/subscribe over bxCAN or FDCAN
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 *
 *   feb::can::start(feb::can::Bus::k1, hfdcan1);   // once, before the scheduler
 *   feb::can::run_publishers();                    // the body of one task
 *   feb::can::Publisher<msg::X> x_tx{fill_x};      // at namespace scope
 *   feb::can::rx<msg::Y>.v().field                 // anywhere
 *
 * Received frames are decoded in the CAN interrupt. Sent frames go to a free
 * hardware slot, or wait in a ring that the TX-complete interrupt drains.
 */

#ifdef FEB_CAN_LIBRARY_H
#error "feb_can.hpp and FEB_CAN_Library cannot be used by the same board"
#endif

#include "feb_can_traits.hpp"
#include "main.h"

#include <atomic>
#include <cstdint>

#if defined(HAL_CAN_MODULE_ENABLED) == defined(HAL_FDCAN_MODULE_ENABLED)
#error "enable exactly one of the CAN and FDCAN HAL modules in the .ioc"
#endif

namespace feb::can
{
#if defined(HAL_FDCAN_MODULE_ENABLED)
using Handle = FDCAN_HandleTypeDef;
#else
using Handle = CAN_HandleTypeDef;
#endif

// The board's buses, numbered in the order it starts them. Which peripheral
// carries each one is decided by start(): Bus::k1 may be CAN2.
enum class Bus : std::uint8_t
{
  k1,
  k2,
};

struct Stats
{
  std::uint32_t tx;         // acknowledged on the bus
  std::uint32_t tx_dropped; // oldest frame overwritten in a full TX ring
  std::uint32_t tx_failed;  // AutoRetransmission off: lost arbitration or got no ACK
  std::uint32_t rx;
  std::uint32_t rx_lost;    // hardware RX FIFO overruns
  std::uint32_t rx_errors;  // length did not match the message
  std::uint32_t bus_off;
};

struct Frame
{
  std::uint32_t id;
  bool extended;
  std::uint8_t length;
  std::uint8_t data[8];
};

// Interrupt context: keep it short and non-blocking.
using FrameHook = void (*)(Bus bus, const Frame &frame);

// False if the subscribers on `bus` need more filters than the peripheral has,
// or the .ioc uses a mode this library does not support (see README).
// With `every_frame`, the bus accepts every frame and passes each one to it
// (before any subscriber); subscribers still work and need no filters.
bool start(Bus bus, Handle &handle, FrameHook every_frame = nullptr);

[[noreturn]] void run_publishers();

Stats stats(Bus bus);

namespace detail
{
bool send(Bus bus, const Frame &frame);

inline std::uint32_t now_ms()
{
  return HAL_GetTick();
}

struct RxBase
{
  RxBase(Bus bus, std::uint32_t id, bool extended);
  RxBase(const RxBase &) = delete;

  // Interrupt context. False if the frame does not decode.
  virtual bool deliver(const std::uint8_t *data, std::uint8_t length) = 0;

  RxBase *const next;
  const Bus bus;
  const std::uint32_t id;
  const bool extended;

protected:
  ~RxBase() = default;
};

struct TxBase
{
  TxBase(std::uint32_t period_ms, std::uint32_t offset_ms);
  TxBase(const TxBase &) = delete;

  virtual void emit() = 0;

  TxBase *next = nullptr;
  const std::uint32_t period_ms;
  std::uint32_t due_ms;

protected:
  ~TxBase() = default;
};
} // namespace detail

template <class M, Bus B = Bus::k1>
class Publisher final : detail::TxBase
{
  static_assert(M::kLength <= sizeof(Frame::data), "classic CAN carries at most 8 bytes");

public:
  // Return false to skip this cycle.
  using Fill = bool (*)(typename M::Data &out);

  // A period of 0 never schedules; call publish() instead.
  explicit Publisher(Fill fill, std::uint32_t period_ms = M::kCycleMs)
      : TxBase(period_ms, period_ms ? M::kFrameId % period_ms : 0), fill_(fill)
  {
  }

  bool publish()
  {
    typename M::Data data{};
    if (!fill_(data))
    {
      return false;
    }
    Frame frame{M::kFrameId, M::kExtended, M::kLength, {}};
    if (M::kPack(frame.data, &data, sizeof(frame.data)) != M::kLength)
    {
      return false;
    }
    return detail::send(B, frame);
  }

private:
  void emit() override { publish(); }

  const Fill fill_;
};

template <class M, Bus B = Bus::k1>
class Subscriber final : detail::RxBase
{
public:
  static constexpr std::uint32_t kStaleMs = 3 * M::kCycleMs;

  Subscriber() : RxBase(B, M::kFrameId, M::kExtended) {}

  // Each field is whole and current; two fields read separately may come
  // from different frames. Use snapshot() when fields must agree.
  const typename M::Data &v() const { return data_; }

  typename M::Data snapshot() const
  {
    for (;;)
    {
      const std::uint32_t before = seq_;
      std::atomic_signal_fence(std::memory_order_seq_cst);
      const typename M::Data out = data_;
      std::atomic_signal_fence(std::memory_order_seq_cst);
      if ((before & 1u) == 0u && before == seq_)
      {
        return out;
      }
    }
  }

  bool fresh() const
  {
    static_assert(M::kCycleMs > 0, "message has no cycle time to judge freshness by");
    return rx_count_ > 0 && detail::now_ms() - last_rx_ms_ < kStaleMs;
  }

  // UINT32_MAX until the first frame arrives.
  std::uint32_t age_ms() const { return rx_count_ > 0 ? detail::now_ms() - last_rx_ms_ : UINT32_MAX; }

  std::uint32_t rx_count() const { return rx_count_; }

private:
  bool deliver(const std::uint8_t *data, std::uint8_t length) override
  {
    typename M::Data decoded;
    if (M::kUnpack(&decoded, data, length) < 0)
    {
      return false;
    }
    seq_ = seq_ + 1;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    data_ = decoded;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    seq_ = seq_ + 1;
    last_rx_ms_ = detail::now_ms();
    rx_count_ = rx_count_ + 1;
    return true;
  }

  typename M::Data data_{};
  volatile std::uint32_t seq_ = 0;
  volatile std::uint32_t last_rx_ms_ = 0;
  volatile std::uint32_t rx_count_ = 0;
};

template <class M, Bus B = Bus::k1>
inline Subscriber<M, B> rx;

} // namespace feb::can
