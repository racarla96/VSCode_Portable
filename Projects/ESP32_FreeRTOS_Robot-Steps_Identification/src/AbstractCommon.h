/**
 * @file AbstractCommon.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
  * 
 * @ Modified by: Rafael Carbonell
 * @ Modified time: 10-12-2021
 * @ Style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

#pragma once

#include <algorithm>

#include <SLHRobot.h>

/**
 * @brief Function to saturate the input signal between the given upper and lower limits.
 * 
 * @param value input signal
 * @param upper upper limit
 * @param lower lower limit
 * @return float saturated signal
 */
float sat(float value, float upper, float lower) {return std::max(lower, std::min(value, upper));}
