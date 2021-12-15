/**
 * @file DCMotorSensorImpl.h
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 13-12-2021
 * 
 * @copyright Copyright (c) 2021
 * 
 * @ Modified by: Rafael Carbonell
 * @ Modified time: 13-12-2021
 * @ Style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

// #define DEBUG

#pragma once

#include <SLHRobot.h>
#include <AbstractDCMotorPosSensor.h>

#include <Arduino.h>
#include <ESP32Encoder.h>

class DCMotorPosSensorImpl : public AbstractDCMotorPosSensor {
    public:
        DCMotorPosSensorImpl(uint8_t in_A, uint8_t in_B, float counts2rad){
            _enc = new ESP32Encoder();
            _enc->attachFullQuad(in_A, in_B);
            _counts2rad = counts2rad;
        }

        float getPosition() {
            _i_position = _enc->getCount();
            return _i_position * _counts2rad;
        }

    private:
        int64_t _i_position;
        float _counts2rad;
        ESP32Encoder * _enc;
};