#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void PlantControlTask(void *pvParameters);

void setup()
{
  // Start serial read task
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      PlantControlTask,    // Function to be called
      "DC Motor Task",     // Name of task
      4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass
      1,                   // Task priority (must be same to prevent lockup)
      NULL,                // Task handle
      app_cpu);            // Run on one core for demo purposes (ESP32 only)

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {}

#include <ZBlock.h>

#include <SLHRobot.h>
#include <DCMotorDriverImpl.h>
#include <DCMotorPosSensorImpl.h>

#include <ErriezINA219.h>
#include <DiscreteLowPassFilter.h>

#define N_MOTORS 4
#define COUNTS2RAD ((2.0f * PI) / (70.0f * 64.0f))

const uint8_t pin_enc[N_MOTORS][2] = // Encoders pinout of ESP32
    {
        {17, 16}, // Motor 1
        {26, 25}, // Motor 2
        {27, 14}, // Motor 3
        {12, 13}, // Motor 4
};

// Default I2C Address 0x40
#define INA219_I2C_ADDRESS 0x40
// 0.1 Ohm shunt resistor
#undef INA219_SHUNT_RESISTOR
#define INA219_SHUNT_RESISTOR 0.01

AbstractDCMotorDriver *Motors[N_MOTORS];
AbstractDCMotorPosSensor *MotorsPosSensors[N_MOTORS];
AbstractDCMotorPlant *DCMotorPlant[N_MOTORS];
float AngPos[N_MOTORS];
float AngPosLast[N_MOTORS];
float AngVelEst[N_MOTORS];
DiscreteLowPassFilter *VBattery;
DiscreteLowPassFilter *AngVelEst_F[N_MOTORS];

INA219 *CVSensor; // Current and voltage sensor
float voltage_bias = -0.1;
float voltage;
float max_voltage = 12;
float min_voltage = -max_voltage;

bool toggle = false;
const int counts_steps = 100;
int counter = 0;
const float step=0.5;
float stepVoltage;
float targetVoltage;
bool finish = false;

// LED
const uint8_t R = 35;
const uint8_t G = 2;
const uint8_t B = 4;

//#define CTRL_TASK_DEBUG
#define CTRL_TASK_CYCLE_RATE_MS 10
const float ctrl_task_period_s = 0.01f;

void PlantControlTask(void *pvParameters)
{
  TickType_t xLastWakeTime;
  bool init = true; // All is initialized correctly

  Serial.begin(115200);

  // Config LED
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  VBattery = new DiscreteLowPassFilter(0.92442789, 0.03778605, 0.03778605);

  for (int i = 0; i < N_MOTORS; i++)
  {
    AngPos[i] = 0;
    AngPosLast[i] = 0;
    AngVelEst[i] = 0;
    AngVelEst_F[i] = new DiscreteLowPassFilter(0.4360604, 0.2819698, 0.2819698);
  }

  for (int i = 0; i < N_MOTORS; i++)
  {
    MotorsPosSensors[i] = new DCMotorPosSensorImpl(pin_enc[i][0], pin_enc[i][1], COUNTS2RAD);
  }

  for (int i = 0; i < N_MOTORS && init; i++)
  {
    Motors[i] = new DCMotorDriverImpl(i + 1);
    init = Motors[i]->init();
  }
  if (!init)
  {
    Serial.println("Initialization of DC Motor Driver error");
    while (1)
    {
      Serial.println("F");
      delay(1000);
      digitalWrite(B, HIGH);   
      delay(1000); 
      digitalWrite(B, LOW);   
      delay(1000);  
    }
  }

  CVSensor = new INA219(INA219_I2C_ADDRESS, INA219_SHUNT_RESISTOR);
  init = CVSensor->begin();
  if (!init)
  {
    Serial.println("Initialization of DFRobot INA219 (Current and Voltage Sensor) error");
    while (1)
    {
      Serial.println("F");
      delay(1000);
      digitalWrite(R, HIGH);   
      delay(1000); 
      digitalWrite(R, LOW);   
      delay(1000);  
    }
  }

  CVSensor->read();
  voltage = CVSensor->busVoltage;
  while(voltage < 12.6) {
    delay(1000);
    CVSensor->read();
    voltage = CVSensor->busVoltage;
    digitalWrite(G, HIGH);   
    delay(1000);  
    CVSensor->read();
    voltage = CVSensor->busVoltage;
    digitalWrite(G, LOW);  
  }


  for (int i = 0; i < N_MOTORS; i++)
  {
    DCMotorPlant[i] = new AbstractDCMotorPlant(Motors[i], MotorsPosSensors[i], 1 / (voltage - 0.6), max_voltage, min_voltage);
  }

  stepVoltage = max_voltage;

  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, CTRL_TASK_CYCLE_RATE_MS);

    CVSensor->read();
    voltage = CVSensor->busVoltage;

    if(toggle && !finish) {
      if (counter == 0) {
        for (int i = 0; i < N_MOTORS; i++)
        {
          DCMotorPlant[i]->setVoltage(stepVoltage);
        }
        targetVoltage = stepVoltage;
        stepVoltage = stepVoltage - step;
        if(stepVoltage < min_voltage) {
          finish = true;
          targetVoltage = 0;
        }
      }
      counter++;
      if(counter == counts_steps){toggle = false; counter = 0;}
    } else {
      if (counter == 0) {
        targetVoltage = 0;
        for (int i = 0; i < N_MOTORS; i++)
        {
          DCMotorPlant[i]->setVoltage(targetVoltage);
        }

      }
      counter++;
      if(counter == counts_steps){toggle = true; counter = 0;}
    }

    for (int i = 0; i < N_MOTORS; i++)
    {
      AngPos[i] = DCMotorPlant[i]->getPosition();
      AngVelEst[i] = (AngPos[i] - AngPosLast[i])/(2.0f*ctrl_task_period_s);
      AngVelEst[i] = -1*AngVelEst_F[i]->step(AngVelEst[i]); // FILTER
      AngPosLast[i] = AngPos[i];
    }

    voltage = VBattery->step(voltage); // FILTER

    Serial.print(xLastWakeTime);
    for (int i = 0; i < N_MOTORS; i++)
    {
      Serial.print(",");
      Serial.print(DCMotorPlant[i]->getVoltage());
    }
    for (int i = 0; i < N_MOTORS; i++)
    {
      Serial.print(",");
      Serial.print(AngVelEst[i]);
    }
    Serial.print(",");
    Serial.print(voltage);
    Serial.print(",");
    Serial.print(counter);
    Serial.println();
  }
  // Will never get here!
}