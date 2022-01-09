/**
 * @file RM_DiscreteLowPassFilter.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 15-12-2021
 * 
 * @copyright Copyright (c) 2021
 * @style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

// [Youtube Channel] Curio Res - DC motor PID speed control - https://www.youtube.com/watch?v=HRaZLCBFVDE
// [Youtube Channel] Curio Res - How to design and implement a digital low-pass filter on an Arduino - https://www.youtube.com/watch?v=HJ-C4Incgpw

#pragma once

#include <RM_CH.hpp>

class DiscreteLowPassFilter {
    public:
        DiscreteLowPassFilter(float a1, float b0, float b1){_a1 = a1; _b0 = b0; _b1 = b1; _yk = 0;  _yk_1 = 0; _uk_1 = 0;}
        void initial_u(float uk){_uk_1 = uk; _yk_1 = uk;}
        float step(float uk){_yk = _a1 * _yk_1 + _b0 * uk + _b1 * _uk_1; _yk_1 = _yk; _uk_1 = uk; return _yk;}
    private:
        float _a1, _b0, _b1;
        float _yk, _yk_1, _uk_1;
};