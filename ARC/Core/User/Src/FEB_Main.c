#include "FEB_Main.h"
#include "main.h"
#include "feb_can_lib.h"
#include "feb_uart.h"
#include "feb_log.h"
#include "cmsis_os.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t canTxQueueHandle;
extern osMessageQueueId_t canRxQueueHandle;
extern osMutexId_t canTxMutexHandle;
extern osMutexId_t canRxMutexHandle;
extern osSemaphoreId_t canTxMailboxSemHandle;
extern osMutexId_t debugUartTxMutexHandle;
extern osMutexId_t debugLogMutexHandle;

static uint8_t debug_tx_buffer[512];
static uint8_t debug_rx_buffer[64];

// TODO: Move this stuff to other files for conciseness
// FEB_Init should initialize CAN usage, TPS usage, Pressure Sensor Reading, FEET setup, etc
void FEB_Init(void)
{
  const FEB_UART_Config_t uart_cfg = {
      .huart = &huart2,
      .hdma_tx = NULL,
      .hdma_rx = NULL,
      .tx_buffer = debug_tx_buffer,
      .tx_buffer_size = sizeof(debug_tx_buffer),
      .rx_buffer = debug_rx_buffer,
      .rx_buffer_size = sizeof(debug_rx_buffer),
      .get_tick_ms = HAL_GetTick,
      .tx_mutex = debugUartTxMutexHandle,
      .enable_rx_queue = false,
      .enable_tx_queue = false,
      .rx_queue = NULL,
      .tx_queue = NULL,
  };
  if (FEB_UART_Init(FEB_UART_INSTANCE_1, &uart_cfg) != FEB_UART_OK)
  {
    Error_Handler();
  }

  const FEB_Log_Config_t log_cfg = {
      .uart_instance = FEB_UART_INSTANCE_1,
      .level = FEB_LOG_INFO,
      .colors = false,
      .timestamps = true,
      .get_tick_ms = HAL_GetTick,
      .custom_output = NULL,
      .mutex = debugLogMutexHandle,
  };
  if (FEB_Log_Init(&log_cfg) != 0)
  {
    Error_Handler();
  }
}

void ARC_CAN_Init(void)
{
  // Initialize can config
  FEB_CAN_Config_t cfg = {
      .hcan1 = &hcan1,
      .hcan2 = &hcan2,
      .get_tick_ms = HAL_GetTick,
#if FEB_CAN_USE_FREERTOS
      .tx_queue = canTxQueueHandle,
      .rx_queue = canRxQueueHandle,
      .tx_mutex = canTxMutexHandle,
      .rx_mutex = canRxMutexHandle,
      .tx_mailbox_sem = canTxMailboxSemHandle,
#endif
  };

  if (FEB_CAN_Init(&cfg) != FEB_CAN_OK)
  {
    LOG_E(TAG_MAIN, "%s", "CAN Initialization failed");
    while (1)
    {
    }
  }
}
