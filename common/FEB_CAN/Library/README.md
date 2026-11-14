# FEB_CAN Library

Typed CAN publish/subscribe for C++ boards, over bxCAN (STM32F4) or FDCAN (STM32U5/G4/H5) in classic mode. It replaces [FEB_CAN_Library](../../FEB_CAN_Library/README.md) one board at a time; a board links one or the other, never both.

Messages come from [FEB_CAN_Definitions](../../FEB_CAN_Definitions/README.md) (`feb::can::msg::*`).

## Use

```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE feb_can_cxx)
```

```cpp
#include "feb_can.hpp"
namespace fc = feb::can;
namespace fm = feb::can::msg;

// Board init, after MX_FDCANx_Init / MX_CANx_Init, before the scheduler:
if (!fc::start(fc::Bus::k1, hfdcan1)) { Error_Handler(); }   // Bus::k1 is just "first bus"; hcan2 works too

// The body of one task:
fc::run_publishers();

// Send: at namespace scope, scheduled at the message's DBC cycle time.
bool fill_heartbeat(feb_can_dash_heartbeat_t &m) { /* fill m */ return true; } // false skips the cycle
fc::Publisher<fm::DashHeartbeat> heartbeat_tx{fill_heartbeat};

// Receive: anywhere.
if (fc::rx<fm::WssRear>.fresh()) { speed = fc::rx<fm::WssRear>.v().wss_left; }
```

| Call | Meaning |
|---|---|
| `rx<M>.v()` | Latest value. Each field is whole and current; two fields read separately may come from different frames. |
| `rx<M>.snapshot()` | Latest value with every field from the same frame. Use when fields belong together (index + value, rolling counter + payload). |
| `rx<M>.fresh()` | Received within 3 cycle times. |
| `rx<M>.age_ms()` | Milliseconds since the last frame; `UINT32_MAX` before the first. |
| `Publisher<M>{fill, period_ms}` | `period_ms` defaults to the DBC cycle; 0 never schedules. |
| `publisher.publish()` | Send now, outside the schedule. |
| `stats(bus)` | TX (acknowledged), TX dropped/failed, RX, RX lost/errors, bus-off counts. |
| `start(bus, handle, hook)` | Accept every frame on the bus and pass each to `hook(bus, frame)` in the interrupt, before subscribers. For loggers. |

The second template argument picks the bus: `rx<M, Bus::k2>`. It defaults to `Bus::k1`.

## How it works

- **Receive.** `start()` builds exact-id hardware filters from every `rx<M>` the board uses. A matching frame is decoded in the CAN interrupt straight into its subscriber. No RX task or queue.
- **Send.** `send` writes to a free hardware TX slot, or queues in a 16-frame ring that the TX-complete interrupt drains. A full ring overwrites its oldest frame and counts it (`tx_dropped`), so what goes out after a stall is the newest data. No TX task, queue, mutex or semaphore.
- **Schedule.** `run_publishers()` checks every publisher each millisecond. Offsets are `frame_id % period`, which spreads messages with the same period. Missed cycles are skipped, not burst.
- **Logging.** With a hook, the bus takes every frame (FDCAN: the global filter accepts unmatched frames; bxCAN: one match-all bank), so subscriber ids need no filters. The hook runs in the interrupt: copy the frame into a queue and return.
- **Bus-off.** bxCAN rejoins by itself (AutoBusOff). FDCAN rejoins from the interrupt. Both count it.

## .ioc requirements

| Peripheral | Setting |
|---|---|
| Either | Only one of the CAN and FDCAN HAL modules enabled. |
| bxCAN | `AutoBusOff = ENABLE`. NVIC: RX0, TX and SCE interrupts. CAN1 filters use banks 0–13, CAN2 banks 14–27, whichever `Bus` they are. |
| FDCAN | `FrameFormat = FDCAN_FRAME_CLASSIC`. NVIC: interrupt 0. `StdFiltersNbr` / `ExtFiltersNbr` set the filter budget. |

`AutoRetransmission` in the .ioc decides retries; enable it unless there is a reason not to. Enabled: a frame retries until acknowledged, so an unacknowledged frame (alone on the bench) holds its slot, the ring fills, and older frames are overwritten (`tx_dropped`). Disabled: one attempt, and a frame that loses arbitration or gets no ACK counts as `tx_failed`.

## Limits

| Limit | Value | Result if exceeded |
|---|---|---|
| Subscribed ids per bus, bxCAN | 28 (2 per bank); unlimited with a hook | `start()` returns false |
| Subscribed ids per bus, FDCAN | 2 × `StdFiltersNbr` standard (max 56), 2 × `ExtFiltersNbr` extended (max 16); unlimited with a hook | `start()` returns false |
| TX ring | 16 frames per bus | Oldest frame overwritten, `tx_dropped` |
| Payload | 8 bytes (classic) | Compile error |

## Files

| File | Holds |
|---|---|
| `Inc/feb_can.hpp` | The whole public API: `start`, `run_publishers`, `stats`, `Publisher`, `rx` |
| `Src/feb_can.cpp` | Subscriber dispatch, TX ring, publisher schedule |
| `Src/feb_can_transport.hpp` | The contract: a transport provides `hw_start` and `hw_write`, and reports through `on_rx`, `on_tx_done`, `on_rx_lost`, `on_bus_off` |
| `Src/Transports/feb_can_bxcan.cpp` | bxCAN transport; compiles to nothing without `HAL_CAN_MODULE_ENABLED` |
| `Src/Transports/feb_can_fdcan.cpp` | FDCAN transport; compiles to nothing without `HAL_FDCAN_MODULE_ENABLED` |
