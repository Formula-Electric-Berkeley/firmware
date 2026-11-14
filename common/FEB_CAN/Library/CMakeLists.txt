add_library(feb_can_cxx INTERFACE)

target_sources(feb_can_cxx INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/feb_can.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Transports/feb_can_bxcan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Transports/feb_can_fdcan.cpp
)

target_include_directories(feb_can_cxx INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FEB_CAN_Definitions/gen
)
