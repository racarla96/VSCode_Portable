/**
 * @file RM_AbstractDCMotorPosSensor.hpp
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