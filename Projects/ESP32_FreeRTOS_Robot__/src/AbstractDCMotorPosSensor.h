/**
 * @file AbstractDCMotorSensor.h
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

#pragma once

#include <SLHRobot.h>
#include <AbstractCommon.h>

/**
 * @brief 
 * 
 */
class AbstractDCMotorPosSensor {
    public:
        AbstractDCMotorPosSensor(){}
        virtual ~AbstractDCMotorPosSensor(){}
        
        /**
         * @brief function to initialize variables and objects if it is not possible to initialize from the child constructor.
         * 
         * @return true returns true when the initialization is correct.
         * @return false returns false when the initialization has some problem.
         */
        virtual bool init() {return true;}

        virtual float getPosition(){return 0;} // [rad/s]
};