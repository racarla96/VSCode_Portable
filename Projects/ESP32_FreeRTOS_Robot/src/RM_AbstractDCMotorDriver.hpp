/**
 * @file RM_AbstractDCMotorDriver.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 15-12-2021
 * 
 * @copyright Copyright (c) 2021
 * @style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

#pragma once

#include <RM_CH.hpp>
#include <RM_AbstractCommonFunctions.hpp>

/**
 * @brief Abstract DC Motor Driver class are a common interface to control a DC driver motor with common and easy functions.
 */
class AbstractDCMotorDriver {
    public:
        /**
         * @brief Standard upper value by convention.
         */
        const float upper_limit = 1.0f;
        
        /**
         * @brief Standard lower value by convention.
         */
        const float lower_limit = -1.0f;

        /**
         * @brief Construct a new Abstract DC Motor Driver object.
         */
        AbstractDCMotorDriver() {}

        /**
         * @brief Destroy the Abstract DC Motor Driver object.
         */
        virtual ~AbstractDCMotorDriver() {}

        /**
         * @brief function to initialize variables and objects if it is not possible to initialize from the child constructor.
         * 
         * @return true returns true when the initialization is correct.
         * @return false returns false when the initialization has some problem.
         */
        virtual bool init() {return true;}

        /**
         * @brief Set the value to the motor output.
         * For a correct working of the class, you need to call the abstract class function, like this:
         * https://www.tutorialspoint.com/how-to-call-a-parent-class-function-from-derived-class-function-in-cplusplus
         * 
         * @param value 
         */
        virtual void setValue(float value) {_value = sat(value, upper_limit, lower_limit);}

        /**
         * @brief Return the value to the motor output.
         * 
         * @return float 
         */
        virtual float getValue() {return _value;}

        /**
         * @brief Applies the order to stop or not apply voltage to the motor.
         * Setting the current value applied to 0.
         */
        virtual void stop() {_value = 0;}

    protected:
        /**
         * @brief Interna variable to store current value of the output of the driver.
         */
        float _value;
};