/**
 * @file AbstractDCMotorPlant.h
 * @author your name (you@domain.com)
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


#pragma once

#include <SLHRobot.h>
#include <AbstractCommon.h>
#include <AbstractDCMotorDriver.h>

/**
 * @brief Abstract DC Motor Plant class are a common interface to control a DC motor with common and easy functions.
 */
class AbstractDCMotorPlant {
    public:

        /**
         * @brief Construct a new Abstract DC Motor Plant object
         */
        AbstractDCMotorPlant(AbstractDCMotorDriver * drv, float gain, float max_voltage, float min_voltage) 
            {_drv = drv; _gain = gain; _max_voltage = max_voltage; _min_voltage = min_voltage;}

        /**
         * @brief Destroy the Abstract DC Motor Plant object
         */

        virtual ~AbstractDCMotorPlant() {}

        /**
         * @brief function to initialize variables and objects if it is not possible to initialize from the child constructor.
         * 
         * @return true returns true when the initialization is correct.
         * @return false returns false when the initialization has some problem.
         */
        virtual bool init() {return true;}

        /**
         * @brief Set the Gain object
         * 
         * @param gain 
         */
        virtual void setGain(float gain) {_gain = gain;}

        /**
         * @brief Get the Gain object
         * 
         * @return float 
         */
        virtual float getGain() {return _gain;}

        /**
         * @brief Set the Voltage object
         * 
         * @param voltage 
         */
        virtual void setVoltage(float voltage) {_voltage = sat(voltage, _max_voltage, _min_voltage); _drv->setValue(_voltage * _gain);}

        /**
         * @brief Get the Voltage object
         * 
         * @return float 
         */
        virtual float getVoltage() {return _voltage;}

        /**
         * @brief Set the Voltage object
         * 
         * @param max_voltage 
         */
        virtual void setMaxVoltage(float max_voltage) {_max_voltage = max_voltage;}

        /**
         * @brief Get the Voltage object
         * 
         * @return float 
         */
        virtual float getMaxVoltage() {return _max_voltage;}

        /**
         * @brief Set the Voltage object
         * 
         * @param min_voltage 
         */
        virtual void setMinVoltage(float min_voltage) {_min_voltage = min_voltage;}

        /**
         * @brief Get the Voltage object
         * 
         * @return float 
         */
        virtual float getMinVoltage() {return _min_voltage;}

        /**
         * @brief Applies the order to stop or not apply voltage to the motor.
         * Setting the current value applied to 0.
         */
        virtual void stop() {_voltage = 0; _drv->stop();}

    protected:
        float _gain;
        float _voltage;
        float _max_voltage;
        float _min_voltage;

        AbstractDCMotorDriver * _drv;
};