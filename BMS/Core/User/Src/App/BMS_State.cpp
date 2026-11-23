#include "feb_can_db.h"
#include <map>
#include "FEB_HW_Relay.h"
#include "FEB_CAN_Charger.h"

// class BMS_State {
//     public:
//         map<bool, BMS_State> transitions;
//         static BMS_State tick(); {
//             // for thing in map
//             //     if key then set your state to value
            
//         }
//         static void evaluate_faults();
//         // Map per state contains transition values to get out of each state
//         // State A: map contains whatever conditions need to be true in order to transition to state B.
        
//         //overridden probably
//         static void enter();

// }

struct state {
    map<bool, BMS_SM_States> transitions;
    // function somehow??
}

typedef enum {
    ORIGIN,
    LV_POWER = new state({{FEB_HW_TSMS_Indicator_Get(), BMS_SM_States.BUS_HEALTH_CHECK}, {FEB_CAN_Charging_Status() == 0, BMS_SM_States.BATTERY_FREE}}, some_method())
    PRECHARGE,
    BUS_HEALTH_CHECK,
    BATTERY_FREE
    // etc
} BMS_SM_States

class StateMachine {
    BMS_SM_States currentState = BMS_SM_States.ORIGIN;
    // static void changeStateTo(BMS_State state) {
    //     currentState = state;
    // }
    for (auto& [key, value] : currentState)
                if (key) {
                    return value
                } else {
                    return 
                }
}

class LVPower : public BMS_State {
    map<bool, BMS_State> transitions = {{FEB_HW_TSMS_Indicator_Get(), busHealthCheck}, {FEB_CAN_Charging_Status() == 0, batteryFree}, {}};


        tick() {
            super.tick(state_map)
        }
}