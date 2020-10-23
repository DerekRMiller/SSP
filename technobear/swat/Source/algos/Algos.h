#pragma once

enum AlgoDef {
    A_DISPLAY,      // AgDisplay    - Display
    A_CONSTANT,     // AgConstant   - Simple
    A_P_ADDER,      // AgPrecAdder  - Simple
    A_MIN_MAX,      // AgMinMax     - Simple
    A_SWITCH,       // AgSwitch     - Simple
    A_EQUAL,        // AgEqual      - Simple
    A_CONSTANT_N,   // AgConstantN  - Musical 
    A_EQUAL_N,      // AgEqualN     - Musical 
    A_DELAY,        // AgDelay      - Delay
    A_MAX
};

#include "AgSimple.h"
#include "AgDisplay.h"
#include "AgMusical.h"
#include "AgDelay.h"