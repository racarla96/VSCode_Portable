/**
 * @file DCMotorDriver_Impl.h
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 10-12-2021
 * 
 * @copyright Copyright (c) 2021
 * 
 * @ Modified by: Rafael Carbonell
 * @ Modified time: 10-12-2021
 * @ Style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

// #define DEBUG

#pragma once

#include <SLHRobot.h>
#include <AbstractDCMotorDriver.h>

#include <stdlib.h>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

class DCMotorDriverImpl : public AbstractDCMotorDriver {
    public:
        DCMotorDriverImpl(uint8_t motor_number) {_motor_number = motor_number;} 

        bool init() {
            #ifdef DEBUG
                Serial.println("Initialization the Adafruit_MotorShield");
            #endif
            AFMS = new Adafruit_MotorShield();
            if(!AFMS->begin()) return false;

            Wire.setTimeOut(2);

            #ifdef DEBUG
                Serial.println("Motor " + _motor_number);
            #endif
            _motor = AFMS->getMotor(_motor_number);
            if(_motor == NULL) return false;

            return true;
        }

        // https://www.tutorialspoint.com/how-to-call-a-parent-class-function-from-derived-class-function-in-cplusplus
        void setValue(float value) {
            AbstractDCMotorDriver::setValue(value);

            _value_c = _value * Driver_Resolution;

            _value_i = (int16_t) _value_c;
            _value_u = (uint16_t) abs(_value_c);

            if(_value_i == 0) {_motor->run(RELEASE);}     
            else if(_value_i > 0) {_motor->run(BACKWARD);}          
            else if(_value_i < 0) { _motor->run(FORWARD);}
            _motor->setSpeedFine(_value_u); 

            #ifdef DEBUG
                char buffer[100];
                sprintf(buffer, "M%d v=%.2f v_sat=%.2f v_i=%4d\n", _motor_number, value, _value, _value_i);
                Serial.write(buffer);
            #endif
        }

    private:

        /* Driver resolution */
        const float Driver_Resolution = 4095.0f;

        float _value_c;
        int16_t _value_i;
        uint16_t _value_u;

        uint8_t _motor_number;
        Adafruit_MotorShield * AFMS;
        Adafruit_DCMotor * _motor;

        
};