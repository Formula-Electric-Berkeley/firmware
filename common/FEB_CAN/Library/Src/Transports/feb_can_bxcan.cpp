/**
 ******************************************************************************
 * @file           : feb_can_bxcan.cpp
 * @brief          : bxCAN transport (STM32F4)
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "../feb_can_transport.hpp"

#ifdef HAL_CAN_MODULE_ENABLED

namespace feb::can::detail
{
namespace
{
// The filter banks are shared: CAN1 owns 0..13, CAN2 owns 14..27.
constexpr std::uint32_t kBanksPerPeripheral = 14;

std::uint32_t filter_word(const FilterId &f)
{
  return f.extended ? (f.id << 3) | CAN_ID_EXT : f.id << 21;
}

// List mode: word2 is a second id. Mask mode: word2 is the mask.
bool add_filter(Handle &h, std::uint32_t bank, std::uint32_t mode, std::uint32_t word1, std::uint32_t word2)
{
  CAN_FilterTypeDef f{};
  f.FilterIdHigh = word1 >> 16;
  f.FilterIdLow = word1 & 0xFFFFU;
  f.FilterMaskIdHigh = word2 >> 16;
  f.FilterMaskIdLow = word2 & 0xFFFFU;
  f.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  f.FilterBank = bank;
  f.FilterMode = mode;
  f.FilterScale = CAN_FILTERSCALE_32BIT;
  f.FilterActivation = CAN_FILTER_ENABLE;
  f.SlaveStartFilterBank = kBanksPerPeripheral;
  return HAL_CAN_ConfigFilter(&h, &f) == HAL_OK;
}

// Two ids per bank, standard and extended mixed; an odd last id fills both slots.
bool add_id_filters(Handle &h, const FilterId *ids, std::size_t count, std::uint32_t bank)
{
  const std::uint32_t end = bank + kBanksPerPeripheral;
  for (std::size_t i = 0; i < count; i += 2)
  {
    const FilterId &second = ids[i + 1 < count ? i + 1 : i];
    if (bank == end || !add_filter(h, bank++, CAN_FILTERMODE_IDLIST, filter_word(ids[i]), filter_word(second)))
    {
      return false;
    }
  }
  return true;
}

void tx_failed_from(CAN_HandleTypeDef *h, std::uint32_t error)
{
  constexpr std::uint32_t kFailBits[] = {
      HAL_CAN_ERROR_TX_ALST0 | HAL_CAN_ERROR_TX_TERR0,
      HAL_CAN_ERROR_TX_ALST1 | HAL_CAN_ERROR_TX_TERR1,
      HAL_CAN_ERROR_TX_ALST2 | HAL_CAN_ERROR_TX_TERR2,
  };
  std::uint32_t failed = 0;
  for (std::uint32_t bits : kFailBits)
  {
    failed += (error & bits) != 0U ? 1U : 0U;
  }
  if (failed != 0U)
  {
    on_tx_done(*h, 0, failed);
  }
}
} // namespace

bool hw_start(Handle &h, bool accept_all, const FilterId *ids, std::size_t count)
{
  if (h.Init.AutoBusOff != ENABLE)
  {
    return false;
  }

  const std::uint32_t first_bank = h.Instance == CAN1 ? 0 : kBanksPerPeripheral;
  const bool filtered = accept_all ? add_filter(h, first_bank, CAN_FILTERMODE_IDMASK, 0, 0) // mask 0: match all
                                   : add_id_filters(h, ids, count, first_bank);

  constexpr std::uint32_t kInterrupts =
      CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_OVERRUN | CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_BUSOFF | CAN_IT_ERROR;
  return filtered && HAL_CAN_ActivateNotification(&h, kInterrupts) == HAL_OK && HAL_CAN_Start(&h) == HAL_OK;
}

bool hw_write(Handle &h, const Frame &frame)
{
  CAN_TxHeaderTypeDef header{};
  header.StdId = frame.extended ? 0 : frame.id;
  header.ExtId = frame.extended ? frame.id : 0;
  header.IDE = frame.extended ? CAN_ID_EXT : CAN_ID_STD;
  header.RTR = CAN_RTR_DATA;
  header.DLC = frame.length;
  std::uint32_t mailbox;
  return HAL_CAN_AddTxMessage(&h, &header, frame.data, &mailbox) == HAL_OK;
}
} // namespace feb::can::detail

using namespace feb::can;
using namespace feb::can::detail;

extern "C"
{
  void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *h)
  {
    CAN_RxHeaderTypeDef header;
    Frame frame{};
    while (HAL_CAN_GetRxFifoFillLevel(h, CAN_RX_FIFO0) > 0 &&
           HAL_CAN_GetRxMessage(h, CAN_RX_FIFO0, &header, frame.data) == HAL_OK) // HAL clamps DLC to 8
    {
      frame.extended = header.IDE == CAN_ID_EXT;
      frame.id = frame.extended ? header.ExtId : header.StdId;
      frame.length = static_cast<std::uint8_t>(header.DLC);
      on_rx(*h, frame);
    }
  }

  void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *h) { on_tx_done(*h, 1, 0); }
  void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *h) { on_tx_done(*h, 1, 0); }
  void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *h) { on_tx_done(*h, 1, 0); }

  // With AutoRetransmission off, a failed attempt arrives here as ALST/TERR.
  void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *h)
  {
    const std::uint32_t error = h->ErrorCode;
    HAL_CAN_ResetError(h);
    if ((error & HAL_CAN_ERROR_RX_FOV0) != 0U)
    {
      on_rx_lost(*h);
    }
    if ((error & HAL_CAN_ERROR_BOF) != 0U)
    {
      on_bus_off(*h); // AutoBusOff rejoins in hardware
    }
    tx_failed_from(h, error);
  }
}

#endif /* HAL_CAN_MODULE_ENABLED */
