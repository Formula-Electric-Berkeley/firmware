/**
 ******************************************************************************
 * @file           : feb_can_fdcan.cpp
 * @brief          : FDCAN transport (STM32U5/G4/H5), classic frames
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "../feb_can_transport.hpp"

#ifdef HAL_FDCAN_MODULE_ENABLED

#include <cstring>

namespace feb::can::detail
{
namespace
{
constexpr std::uint32_t kTxBuffers = FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2;

std::uint32_t count(std::uint32_t buffers)
{
  return static_cast<std::uint32_t>(__builtin_popcount(buffers));
}

bool add_filter(Handle &h, std::uint32_t index, bool extended, std::uint32_t id1, std::uint32_t id2)
{
  FDCAN_FilterTypeDef f{};
  f.IdType = extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  f.FilterIndex = index;
  f.FilterType = FDCAN_FILTER_DUAL;
  f.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  f.FilterID1 = id1;
  f.FilterID2 = id2;
  return HAL_FDCAN_ConfigFilter(&h, &f) == HAL_OK;
}

// Two ids of one type per dual-ID filter element; standard and extended have separate lists.
bool add_filters(Handle &h, const FilterId *ids, std::size_t count, bool extended, std::uint32_t capacity)
{
  std::uint32_t index = 0;
  const FilterId *pending = nullptr;
  for (const FilterId *f = ids; f != ids + count; f++)
  {
    if (f->extended != extended)
    {
      continue;
    }
    if (pending == nullptr)
    {
      pending = f;
      continue;
    }
    if (index == capacity || !add_filter(h, index++, extended, pending->id, f->id))
    {
      return false;
    }
    pending = nullptr;
  }
  return pending == nullptr || (index < capacity && add_filter(h, index, extended, pending->id, pending->id));
}
} // namespace

bool hw_start(Handle &h, bool accept_all, const FilterId *ids, std::size_t count)
{
  constexpr std::uint32_t kInterrupts = FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_MESSAGE_LOST |
                                        FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_ABORT_COMPLETE | FDCAN_IT_BUS_OFF;
  const std::uint32_t unmatched = accept_all ? FDCAN_ACCEPT_IN_RX_FIFO0 : FDCAN_REJECT;
  return h.Init.FrameFormat == FDCAN_FRAME_CLASSIC &&
         (accept_all || (add_filters(h, ids, count, false, h.Init.StdFiltersNbr) &&
                         add_filters(h, ids, count, true, h.Init.ExtFiltersNbr))) &&
         HAL_FDCAN_ConfigGlobalFilter(&h, unmatched, unmatched, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE) == HAL_OK &&
         HAL_FDCAN_ActivateNotification(&h, kInterrupts, kTxBuffers) == HAL_OK && HAL_FDCAN_Start(&h) == HAL_OK;
}

bool hw_write(Handle &h, const Frame &frame)
{
  FDCAN_TxHeaderTypeDef header{};
  header.Identifier = frame.id;
  header.IdType = frame.extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  header.TxFrameType = FDCAN_DATA_FRAME;
  header.DataLength = frame.length; // classic DLC codes equal the byte count
  header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header.BitRateSwitch = FDCAN_BRS_OFF;
  header.FDFormat = FDCAN_CLASSIC_CAN;
  header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  return HAL_FDCAN_AddMessageToTxFifoQ(&h, &header, frame.data) == HAL_OK;
}
} // namespace feb::can::detail

using namespace feb::can;
using namespace feb::can::detail;

extern "C"
{
  void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *h, uint32_t interrupts)
  {
    if ((interrupts & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) != 0U)
    {
      on_rx_lost(*h);
    }
    FDCAN_RxHeaderTypeDef header;
    std::uint8_t data[64]; // HAL copies up to 64 bytes for a classic frame with DLC 9..15
    while (HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0) > 0 &&
           HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &header, data) == HAL_OK)
    {
      Frame frame{};
      frame.id = header.Identifier;
      frame.extended = header.IdType == FDCAN_EXTENDED_ID;
      frame.length = header.DataLength > 8U ? 8U : static_cast<std::uint8_t>(header.DataLength);
      std::memcpy(frame.data, data, frame.length);
      on_rx(*h, frame);
    }
  }

  void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *h, uint32_t buffers)
  {
    on_tx_done(*h, count(buffers), 0);
  }

  // With AutoRetransmission off, a failed attempt ends as a cancellation.
  void HAL_FDCAN_TxBufferAbortCallback(FDCAN_HandleTypeDef *h, uint32_t buffers)
  {
    on_tx_done(*h, 0, count(buffers));
  }

  void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *h, uint32_t interrupts)
  {
    if ((interrupts & FDCAN_IT_BUS_OFF) != 0U && (h->Instance->PSR & FDCAN_PSR_BO) != 0U)
    {
      on_bus_off(*h);
      CLEAR_BIT(h->Instance->CCCR, FDCAN_CCCR_INIT); // rejoins after 129 x 11 recessive bits
    }
  }
}

#endif /* HAL_FDCAN_MODULE_ENABLED */
