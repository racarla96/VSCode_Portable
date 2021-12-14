/**
 * @file InterTaskTypes.h
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 14-12-2021
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// https://techtutorialsx.com/2017/08/20/esp32-arduino-freertos-queues/

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

struct UDPPosition
{
    float X;
    float Y;
};

#define UDPPositionQueueSize 5
QueueHandle_t UDPPositionQueue;

bool initUDPPositionQueue()
{
  UDPPositionQueue = xQueueCreate( UDPPositionQueueSize, sizeof(struct UDPPosition) );
  if(UDPPositionQueue == NULL) return false;
  return true;
}

