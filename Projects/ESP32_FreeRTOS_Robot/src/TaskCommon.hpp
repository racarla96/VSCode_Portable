/**
 * @file TaskCommon.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief 
 * @version 0.1
 * @date 15-12-2021
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

/* Declare a variable to hold the created event group. */
// EventGroup to notify WiFi ON or OFF
EventGroupHandle_t WiFiEventGroup;
#define BIT_0	( 1 << 0 )

bool initWiFiEventGroup(){
  /* Attempt to create the event group. */
  WiFiEventGroup = xEventGroupCreate();

  /* Was the event group created successfully? */
  if( WiFiEventGroup == NULL ) return false;
  return true;
}