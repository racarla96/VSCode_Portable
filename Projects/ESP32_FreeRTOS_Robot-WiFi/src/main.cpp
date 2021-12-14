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

#define DEBUG

#include <InterTaskTypes.h>
#include <WiFiTask.h>

#define CTRL_TASK_CYCLE_RATE_MS 10 // 0.01[s]
void CTRLTask(void *pvParameters)
{
  // Queue data
  struct UDPPosition UDPElement;

  // Periodic Task
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    vTaskDelayUntil(&xLastWakeTime, CTRL_TASK_CYCLE_RATE_MS);
    if (xQueueReceive(UDPPositionQueue, &UDPElement, 0) == pdTRUE) // New data
    {
      Serial.print(xLastWakeTime);
      Serial.print(" ");
      Serial.print(UDPElement.X);
      Serial.print(" ");
      Serial.print(UDPElement.Y);
      Serial.println();
    } 
  }
}

void setup()
{

#ifdef DEBUG
  // Initialize the Serial in a common task
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS); // prevents usb driver crash on startup, do not omit this
  while (!Serial)
    ; // Wait for Serial terminal to open port before starting program
#endif

  if (!initUDPPositionQueue())
  {
    Serial.println("[Arduino Main Task - Setup] Error Creating Queue");
    while (1)
    {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      Serial.println("[Arduino Main Task - Setup] Reset Arduino or check your code");
    }
  }

  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
      WiFiTask,             // Function to be called
      "WiFi Task",          // Name of task
      4096,                 // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                 // Parameter to pass
      tskIDLE_PRIORITY + 1, // Task priority (must be same to prevent lockup)
      NULL,                 // Task handle
      app_cpu);             // Run on one core for demo purposes (ESP32 only)

  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
      CTRLTask,             // Function to be called
      "Ctrl Task",          // Name of task
      4096,                 // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                 // Parameter to pass
      tskIDLE_PRIORITY + 2, // Task priority (must be same to prevent lockup)
      NULL,                 // Task handle
      app_cpu);             // Run on one core for demo purposes (ESP32 only)
}

void loop() { vTaskDelay(10000 / portTICK_PERIOD_MS); }