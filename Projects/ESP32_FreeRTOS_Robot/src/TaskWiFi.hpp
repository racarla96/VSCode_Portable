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
#define DEBUG_WIFI
#endif

#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "NETGEAR95"
#define WIFI_PASS "admin123"
#define WIFI_TIMEOUT_MS 20000       // 20[s]
#define WIFI_TASK_CYCLE_RATE_MS 100 // 0.1[s]

#define WIFI_RECONNECT_ATTEMPTS 5

#include <TaskCommon.hpp>

#include <Arduino.h>
#include <WiFiUDP.h>
WiFiUDP UDP;

#define UDP_ADDR "192.168.1.6"
#define UDP_PORT 514
#define UDP_RX_PACKET_MAX_SIZE 50
#define UDPPOSE_TASK_CYCLE_RATE_MS 500 // 0.2[s]

#include <TaskCommon.hpp>

#define DEBUG_UDP_MESSAGE

/**
 * @brief
 *
 * @param pvParameters
 */
void TaskWiFi(void *pvParameters)
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

    // WiFi Connection Logic
    bool init_wifi_connected = true;
    bool end_task = false; // By WiFi not found
    int n_reconnect_attempts = -1;
    TickType_t xLastAttemptTime;

    // Periodic Task
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    xLastAttemptTime = xLastWakeTime;
    while (!end_task)
    {
        vTaskDelayUntil(&xLastWakeTime, WIFI_TASK_CYCLE_RATE_MS);
        if (WiFi.status() == WL_CONNECTED)
        {
            n_reconnect_attempts = 0;

            if (init_wifi_connected)
            {
                // Notification to the group who wants to use WiFi
                    /* Set bit 0 and bit 4 in xEventGroup. */
                    xEventGroupSetBits(WiFiEventGroup, /* The event group being updated. */
                                                BIT_0);             /* The bits being set. */
#ifdef DEBUG_WIFI
                Serial.println();
                Serial.println();
                Serial.println("[WIFI] Connected: ");
                Serial.print("[WIFI] IP Address: ");
                Serial.print(WiFi.localIP());
                Serial.println();
                Serial.println();
                init_wifi_connected = false;
#endif
                                UDP.begin((uint16_t) UDP_PORT);


            }


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
        else if (WiFi.status() != WL_CONNECTED)
        { // Reconnect
            if (xLastAttemptTime <= xLastWakeTime)
            {
#ifdef DEBUG_WIFI
                Serial.println();
                Serial.println();
                if (n_reconnect_attempts > 0)
                    Serial.println("[WIFI] Connection Timeout - Reconnecting");
                init_wifi_connected = true;
                Serial.println("[WIFI] Connecting to ...");
                Serial.print("[WIFI] ");
                Serial.println(WIFI_SSID);
#endif
                WiFi.mode(WIFI_STA);
                WiFi.begin(WIFI_SSID, WIFI_PASS);
                xLastAttemptTime += WIFI_TIMEOUT_MS / portTICK_PERIOD_MS;

                if (n_reconnect_attempts == 0)
                    /* Set bit 0 and bit 4 in xEventGroup. */
                    xEventGroupSetBits(WiFiEventGroup, /* The event group being updated. */
                                                0);             /* The bits being set. */

                n_reconnect_attempts++;
                if (n_reconnect_attempts > WIFI_RECONNECT_ATTEMPTS)
                    end_task = true;
            }
#ifdef DEBUG_WIFI
            else
            {
                Serial.print(".");
            }
#endif
        }
    }
}
