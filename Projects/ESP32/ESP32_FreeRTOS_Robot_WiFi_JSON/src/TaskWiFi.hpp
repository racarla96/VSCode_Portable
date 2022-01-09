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

#define DEBUG_WIFI

#pragma once

#include <Arduino.h>
#include <WiFi.h>

//#define WIFI_SSID "NETGEAR95"
//#define WIFI_PASS "admin123"
// #define WIFI_SSID "Rafael"
// #define WIFI_PASS "Rafael@2018"
#define WIFI_SSID "MiFibra-AE3A"
#define WIFI_PASS "tyVuRKUa"
#define WIFI_TIMEOUT_MS 20000       // 20 [s]
#define WIFI_TASK_CYCLE_RATE_MS 100 // 0.1 [s]

#define WIFI_RECONNECT_ATTEMPTS 5

#include <WiFiUDP.h>

// UDP UI COMUNICATION
#include <ArduinoJson.h>
#define DEBUG_UDP_UI_MESSAGE
#define UDP_UI_RECV_PORT 34929
#define UDP_UI_SEND_PORT 3560
#define UDP_UI_RECV_PACKET_MAX_SIZE 50
#define UDP_UI_SEND_PACKET_MAX_SIZE 20

#include <TaskCommon.hpp>

/**
 * @brief
 *
 * @param pvParameters
 */
void TaskWiFi(void *pvParameters)
{
    // UDP UI COMUNICATION
    StaticJsonDocument<UDP_UI_RECV_PACKET_MAX_SIZE * 4> JSON_UI_RECV_BUFFER;
    uint8_t UDP_UI_SEND_BUFFER[UDP_UI_SEND_PACKET_MAX_SIZE] = "Pose"; // Buffer to transmit with constant message
    uint8_t UDP_UI_RECV_BUFFER[UDP_UI_RECV_PACKET_MAX_SIZE];
    WiFiUDP UDP_UI;
    bool UDP_UI_NEW_PACKET = false;
    UDP_PACKET_REFERENCES UDP_UI_MSG_VALUES;

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
#ifdef DEBUG_WIFI
            if (init_wifi_connected)
            {
                Serial.println();
                Serial.println();
                Serial.println("[WIFI] Connected: ");
                Serial.print("[WIFI] IP Address: ");
                Serial.print(WiFi.localIP());
                Serial.println();
                Serial.println();
            }
#endif

            // UDP UI COMUNICATION
            if (init_wifi_connected)
                UDP_UI.begin((uint16_t)UDP_UI_RECV_PORT);

            if (UDP_UI.parsePacket() > 0)
            {
                UDP_UI.read(UDP_UI_RECV_BUFFER, UDP_UI_RECV_PACKET_MAX_SIZE);
                UDP_UI_NEW_PACKET = true;
            }
            // #ifdef DEBUG_UDP_UI_MESSAGE
            //             if (UDP_UI_NEW_PACKET)
            //             {
            //                 Serial.print("[WIFI - UDP UI] ");
            //                 Serial.write(UDP_UI_RECV_BUFFER, UDP_UI_RECV_PACKET_MAX_SIZE);
            //                 Serial.println();
            //             }
            // #endif
            // Decoding the message and answer
            if (UDP_UI_NEW_PACKET)
            {
                UDP_UI.beginPacket(UDP_UI.remoteIP(), UDP_UI_SEND_PORT);
                UDP_UI.write(UDP_UI_SEND_BUFFER, UDP_UI_SEND_PACKET_MAX_SIZE);
                UDP_UI.endPacket();

                DeserializationError error = deserializeJson(JSON_UI_RECV_BUFFER, UDP_UI_RECV_BUFFER);
#ifdef DEBUG_UDP_UI_MESSAGE
                if (error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                }
#endif
                if (!error)
                {
                    UDP_UI_MSG_VALUES.ID = JSON_UI_RECV_BUFFER["ID"];
                    UDP_UI_MSG_VALUES.R_vX = JSON_UI_RECV_BUFFER["R_vX"];
                    UDP_UI_MSG_VALUES.R_vY = JSON_UI_RECV_BUFFER["R_vY"];
                    UDP_UI_MSG_VALUES.R_wO = JSON_UI_RECV_BUFFER["R_wO"];
#ifdef DEBUG_UDP_UI_MESSAGE
                    Serial.print("[WIFI - UDP UI] ");
                    Serial.print(" ID: ");
                    Serial.print(UDP_UI_MSG_VALUES.ID);
                    Serial.print(" R_vX: ");
                    Serial.print(UDP_UI_MSG_VALUES.R_vX);
                    Serial.print(" R_vY: ");
                    Serial.print(UDP_UI_MSG_VALUES.R_vY);
                    Serial.print(" R_wO: ");
                    Serial.print(UDP_UI_MSG_VALUES.R_wO);
                    Serial.println();
#endif

                    if( xSemaphoreTake( SEM_REFERENCES, ( TickType_t ) 0 ) == pdTRUE )
                    {
                        /* We were able to obtain the semaphore and can now access the
                        shared resource. */
                        UDP_REFERENCES = UDP_UI_MSG_VALUES;

                        /* We have finished accessing the shared resource.  Release the
                        semaphore. */
                        xSemaphoreGive( SEM_REFERENCES );
                    }

                }

                UDP_UI_NEW_PACKET = false;
            }
            // END -- UDP UI COMUNICATION

            // // READ CAMERA POSITION
            //             if (UDP_UI_NEW_PACKET)
            //             {
            // #ifdef DEBUG_UDP_MESSAGE
            //                 Serial.print("[WIFI - UDP] ");
            // #endif
            //                 int i = 0;
            //                 char *d = strtok((char *)UDP_UI_RECV_BUFFER, ",");
            //                 while (d != NULL)
            //                 {
            //                     UDP_msg_values[i] = atof(d);
            //                     d = strtok(NULL, ",");
            // #ifdef DEBUG_UDP_MESSAGE
            //                     Serial.print(UDP_msg_values[i]);
            //                     Serial.print(" ");
            // #endif
            //                     i++;
            //                 }
            // #ifdef DEBUG_UDP_MESSAGE
            //                 Serial.println();
            // #endif
            //                 UDPElement.X = UDP_msg_values[1];
            //                 UDPElement.Y = UDP_msg_values[2];
            //                 // xQueueSend(UDPPositionQueue, &UDPElement, 0);
            //             }

            //                 // This initializes udp and transfer buffer
            //                 UDP.beginPacket(UDP.remoteIP(), UDP_UI_SEND_PORT);
            //                 UDP.write(UDP_TX_buffer, 10);
            //                 UDP.endPacket();

            if (init_wifi_connected)
            {
                init_wifi_connected = false;
                n_reconnect_attempts = 0;
            }
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
