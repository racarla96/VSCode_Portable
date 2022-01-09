/**
 * @file RM_AbstractCommonFunctions.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 15-12-2021
 * 
 * @copyright Copyright (c) 2021
 * @style: https://www.javaguides.net/2018/08/java-standard-naming-conventions.html
 */

#pragma once

#include <algorithm>

#include <RM_CH.hpp>

/**
 * @brief Function to saturate the input signal between the given upper and lower limits.
 * 
 * @param value input signal
 * @param upper upper limit
 * @param lower lower limit
 * @return float saturated signal
 */
float sat(float value, float upper, float lower) {return std::max(lower, std::min(value, upper));}
