/**
 * @file WiFiTask.h
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief
 * @version 0.1
 * @date 14-12-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

// https://www.arduino.cc/en/Reference/WiFiStatus
// https://www.youtube.com/watch?v=YSGPcm-qxDA

// #define DEBUG_WIFI
// #define DEBUG_UDP_MESSAGE

#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "NETGEAR95"
#define WIFI_PASS "admin123"
#define WIFI_TIMEOUT_MS 20000       // 20[s]
#define WIFI_TASK_CYCLE_RATE_MS 200 // 0.2[s]

#define WIFI_RECONNECT_ATTEMPTS 5

#include <WiFiUDP.h>
WiFiUDP UDP;

#define UDP_ADDR "192.168.1.6"
#define UDP_PORT 514
#define UDP_RX_PACKET_MAX_SIZE 50

#include <InterTaskTypes.h>

/**
 * @brief
 *
 * @param pvParameters
 */
void WiFiTask(void *pvParameters)
{
    // UDP Buffers
    uint8_t UDP_TX_buffer[9] = "Pose"; // Buffer to transmit with constant message
    uint8_t UDP_RX_buffer[UDP_RX_PACKET_MAX_SIZE];
    float UDP_msg_values[7];
    int UDP_RX_size;
    bool UDP_new_packet = false;

    // WiFi Connection Logic
    bool init_wifi_connected = true;
    bool end_task = false; // By WiFi not found
    int n_reconnect_attempts = -1;
    TickType_t xLastAttemptTime;

    // Queue data
    struct UDPPosition UDPElement;

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
                UDP.begin((uint16_t)UDP_PORT);
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
            }
            // This initializes udp and transfer buffer
            UDP.beginPacket(UDP_ADDR, UDP_PORT);
            UDP.write(UDP_TX_buffer, 10);
            UDP.endPacket();

            vTaskDelay((WIFI_TASK_CYCLE_RATE_MS / 2) / portTICK_PERIOD_MS);

            UDP_RX_size = UDP.parsePacket();
            if (UDP_RX_size > 0)
            {
                UDP.read(UDP_RX_buffer, UDP_RX_PACKET_MAX_SIZE);
                UDP_new_packet = true;
            }
#ifdef DEBUG_WIFI
            if (UDP_new_packet)
            {
                Serial.print("[WIFI] ");
                Serial.write(UDP_RX_buffer, UDP_RX_size);
                Serial.println();
            }
#endif
            if (UDP_new_packet)
            {
#ifdef DEBUG_UDP_MESSAGE
                Serial.print("[WIFI - UDP] ");
#endif
                int i = 0;
                char *d = strtok((char *)UDP_RX_buffer, ",");
                while (d != NULL)
                {
                    UDP_msg_values[i] = atof(d);
                    d = strtok(NULL, ",");
#ifdef DEBUG_UDP_MESSAGE
                    Serial.print(UDP_msg_values[i]);
                    Serial.print(" ");
#endif
                    i++;
                }
#ifdef DEBUG_UDP_MESSAGE
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
