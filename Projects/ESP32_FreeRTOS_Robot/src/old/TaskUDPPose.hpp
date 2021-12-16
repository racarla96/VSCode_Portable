/**
 * @file WiFiTask.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief
 * @version 0.1
 * @date 14-12-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

// https://www.arduino.cc/en/Reference/WiFiStatus
// [Youtube Channel] Simply Explained - Keep WiFi Connection Alive with FreeRTOS Task (ESP32 + Arduino series) - https://www.youtube.com/watch?v=YSGPcm-qxDA

#ifdef DEBUG_ALL
//#define DEBUG_UDP
#define DEBUG_UDP_MESSAGE
//#define DEBUG_UDP_MESSAGE_SPLIT
#endif

#pragma once

#include <Arduino.h>
#include <WiFiUDP.h>
WiFiUDP UDP;

#define UDP_ADDR "192.168.1.6"
#define UDP_PORT 514
#define UDP_RX_PACKET_MAX_SIZE 50
#define UDPPOSE_TASK_CYCLE_RATE_MS 500 // 0.2[s]

#include <TaskCommon.hpp>

#include "AsyncUDP.h"


/**
 * @brief
 *
 * @param pvParameters
 */
void TaskUDPPose(void *pvParameters)
{
    // UDP Buffers
    uint8_t UDP_TX_buffer[9] = "Pose"; // Buffer to transmit with constant message
    uint8_t UDP_RX_buffer[UDP_RX_PACKET_MAX_SIZE];
    float UDP_msg_values[7];
    int UDP_RX_size;
    bool UDP_init = false;
    bool UDP_new_packet = false;

    // Queue data
    struct UDPPosition UDPElement;

    EventBits_t uxBits;

    // Periodic Task
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, UDPPOSE_TASK_CYCLE_RATE_MS);
        uxBits = xEventGroupGetBits(WiFiEventGroup);

#ifdef DEBUG_UDP
        Serial.print("[UDPPose] ");
        Serial.print("Checking Wifi Event Group Status: ");
        Serial.print(uxBits);
        Serial.println();
#endif

        if (uxBits > 0)
        {
            if (UDP_init) // Init local port to receive
                UDP.begin((uint16_t) UDP_PORT);

#ifdef DEBUG_UDP_MESSAGE
            Serial.print("[UDPPose] ");
            Serial.print("Sending message!");
            Serial.println();
#endif
            // This initializes udp and transfer buffer
            UDP.beginPacket(UDP_ADDR, UDP_PORT);
            UDP.write(UDP_TX_buffer, 10);
            UDP.endPacket();

            vTaskDelay((UDPPOSE_TASK_CYCLE_RATE_MS / 2) / portTICK_PERIOD_MS);

            UDP_RX_size = UDP.parsePacket();
            if (UDP_RX_size > 0)
            {
                UDP.read(UDP_RX_buffer, UDP_RX_PACKET_MAX_SIZE);
                UDP_new_packet = true;
            }
#ifdef DEBUG_UDP_MESSAGE
            if (UDP_new_packet)
            {
                Serial.print("[UDPPose] ");
                Serial.write(UDP_RX_buffer, UDP_RX_size);
                Serial.println();
            }
#endif
            if (UDP_new_packet)
            {
#ifdef DEBUG_UDP_MESSAGE_SPLIT
                Serial.print("[UDPPose] ");
#endif
                int i = 0;
                char *d = strtok((char *)UDP_RX_buffer, ",");
                while (d != NULL)
                {
                    UDP_msg_values[i] = atof(d);
                    d = strtok(NULL, ",");
#ifdef DEBUG_UDP_MESSAGE_SPLIT
                    Serial.print(UDP_msg_values[i]);
                    Serial.print(" ");
#endif
                    i++;
                }
#ifdef DEBUG_UDP_MESSAGE_SPLIT
                Serial.println();
#endif
                UDPElement.X = UDP_msg_values[1];
                UDPElement.Y = UDP_msg_values[2];
                xQueueSend(UDPPositionQueue, &UDPElement, 0);
            }

            UDP_new_packet = false;
        }
        else
        {
            UDP_init = false;
        }


        
    }
}
