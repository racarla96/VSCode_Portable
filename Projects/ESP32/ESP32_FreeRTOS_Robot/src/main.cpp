#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// https://wiki.seeedstudio.com/Software-FreeRTOS/

#define DEBUG_ALL
#define DEBUG_MAIN

#include <TaskCommon.hpp>
#include <TaskWiFi.hpp>
#include <TaskControl.hpp>

void setup()
{

#ifdef DEBUG_ALL
  // Initialize the Serial in a common task
  Serial.begin(115200);

  vTaskDelay(5000 / portTICK_PERIOD_MS); // prevents usb driver crash on startup, do not omit this
  while (!Serial)
    ; // Wait for Serial terminal to open port before starting program
#endif

  if (!init_semaphores())
  {
#ifdef DEBUG_MAIN
    Serial.println("[Arduino Main Task - Setup] Error initializing the semaphores");
#endif
    while (1)
    {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
#ifdef DEBUG_MAIN
      Serial.println("[Arduino Main Task - Setup] Reset Arduino or check your code");
#endif
    }
  }

  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
      TaskWiFi,             // Function to be called
      "WiFi Task",          // Name of task
      4096,                 // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                 // Parameter to pass
      tskIDLE_PRIORITY + 1, // Task priority (must be same to prevent lockup)
      NULL,                 // Task handle
      app_cpu);             // Run on one core for demo purposes (ESP32 only)

  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
      MotorsControlTask,     // Function to be called
      "Robot Task",          // Name of task
      8192,                 // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                 // Parameter to pass
      tskIDLE_PRIORITY + 2, // Task priority (must be same to prevent lockup)
      NULL,                 // Task handle
      app_cpu);             // Run on one core for demo purposes (ESP32 only)
}

void loop() { vTaskDelay(10000 / portTICK_PERIOD_MS); }