/**
 ******************************************************************************
 * @file           : PCU_Tasks.cpp
 * @brief          : Every FreeRTOS task body on PCU
 * @author         : Formula Electric @ Berkeley
 ******************************************************************************
 */

#include "App/PCU_Torque.h"
#include "PCU_ADC.hpp"
#include "PCU_CAN.h"
#include "PCU_Commands.h"

#include "cmsis_os2.h"
#include "feb_can_tasks.hpp"
#include "feb_uart.h"

namespace fc = feb::can;

extern "C"
{
  void StartPCUTask(void *argument)
  {
    (void)argument;

    PCU_ADC_Init();

    for (;;)
    {
      PCU_Tick();
      osDelay(1);
    }
  }

  void StartUartRxTask(void *argument)
  {
    (void)argument;

    char line_buf[FEB_UART_QUEUE_LINE_SIZE];
    size_t line_len;

    for (;;)
    {
      FEB_UART_ProcessRx(FEB_UART_INSTANCE_1);

      if (FEB_UART_QueueReceiveLine(FEB_UART_INSTANCE_1, line_buf, sizeof(line_buf), &line_len, 10))
      {
        PCU_Console_ProcessLine(line_buf, line_len);
      }
      osDelay(1);
    }
  }

  void StartCanRxTask(void *argument)
  {
    (void)argument;
    fc::RunRxTask(&PCU_CAN_Init);
  }

  void StartCanPubTask(void *argument)
  {
    (void)argument;
    fc::RunPubTask(&PCU_CAN_IsReady);
  }

  void StartCanTxTask(void *argument)
  {
    (void)argument;
    fc::RunTxTask();
  }
}
