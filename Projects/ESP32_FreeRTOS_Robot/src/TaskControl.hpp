/**
 * @file TaskControl.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief
 * @version 0.1
 * @date 15-12-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

// #define DEBUG_CTRL
#define DEBUG_CTRL_PID_MOTORS

#pragma once

#include <Arduino.h>
#include <RM_DiscreteLowPassFilter.hpp>

// LED RGB pinout
// const uint8_t LED_R = 35;
const uint8_t LED_G = 2;
const uint8_t LED_B = 4;

// MOTORS
#include <RM_CH.hpp>
#include <DCMotorDriverImpl.hpp>
#include <DCMotorPosSensorImpl.hpp>
#define N_MOTORS 4
#define COUNTS2RAD ((2.0f * PI) / (70.0f * 64.0f))

const uint8_t DC_ENC[N_MOTORS][2] = // Encoders pinout of ESP32
    {
        {17, 16}, // Motor 1
        {26, 25}, // Motor 2
        {12, 13}, // Motor 3
        {27, 14}, // Motor 4
};

const float DC_max_voltage = 12;
const float DC_min_voltage = -DC_max_voltage;
float DC_AngPos[N_MOTORS];
float DC_AngPosLast[N_MOTORS];
float DC_AngVelEst[N_MOTORS];
DiscreteLowPassFilter *DC_AngVelEst_F[N_MOTORS];
AbstractDCMotorDriver *DC_Motors[N_MOTORS];
AbstractDCMotorPosSensor *DC_MotorsPosSensors[N_MOTORS];
AbstractDCMotorPlant *DC_MotorPlant[N_MOTORS];
// Input filtered
DiscreteLowPassFilter *DC_IN[N_MOTORS];

// BATTERY SENSOR
#include <ErriezINA219.h>
const float VBatt_ON = 12; // Voltage to consider TURN ON the ROBOT
DiscreteLowPassFilter *VBatt;
INA219 *CVSensor; // Current and voltage sensor

// Default I2C Address 0x40
#define INA219_I2C_ADDRESS 0x40
// 0.1 Ohm shunt resistor
#undef INA219_SHUNT_RESISTOR
#define INA219_SHUNT_RESISTOR 0.01

// IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
const float Angle360DEG = 360.0f;
const float AngleDEG2RAD = (2.0f * PI) / Angle360DEG;
const float AngleTolerance = 180;

// TIME
#define CTRL_TASK_CYCLE_RATE_MS 10
const float ctrl_task_period_s = 0.01f;

// CONTROLLERS
#include <ZPID.h>
const float CTRL_Kp = 0.825;
const float CTRL_Ki = 8.25;
const float CTRL_Ts = ctrl_task_period_s;
float DC_MOTOR_REF[N_MOTORS];
float DC_MOTOR_U[N_MOTORS];
ZPID *CTRL_PID[N_MOTORS];

// EKF
#include <ArduinoEigenDense.h>
using namespace Eigen;
#define NSTATES 14
#define NMESURES 8

void MotorsControlTask(void *pvParameters)
{
    bool init = true; // All is initialized correctly

    // ROBOT 
    float X = 0; // X
    float Y = 0; // Y
    float Z = 0; // Theta
    float vX = 0; 
    float vY = 0; 
    float wZ = 0;
    float REF_vX = 0; 
    float REF_vY = 0; 
    float REF_wZ = 0;
    const float ROBOT_L = 0.15f;
    const float ROBOT_W = 0.15f;
    const float ROBOT_R = 0.05f;
    const float IKMixer[N_MOTORS][3] = {{1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, -1.0f}};
    const float FKMixer[3][N_MOTORS] = {{1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, 1.0f, -1.0f}};

    // EKF
    MatrixXd P(NSTATES, NSTATES);
    P << MatrixXd::Zero(NSTATES, NSTATES);

    MatrixXd x(NSTATES, 1);
    x << MatrixXd::Zero(NSTATES, 1);

    MatrixXd D(NMESURES, NMESURES);
    D << MatrixXd::Zero(NMESURES, NMESURES);

    /** LED **/
    // pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);

    /** IMU **/
    float Angle = 0;
    float LastAngle = 0;
    float AngleDiff = 0;
    float AngleCounts = 0;
    float AngleOutput = 0;
    sensors_event_t event;
    Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);
    init = bno.begin();
    if (!init)
    {
#ifdef DEBUG_CTRL
        Serial.println("[Control Task] Initialization of BNO 055 error or not found - check the I2C Address");
        while (1)
        {
            digitalWrite(LED_B, HIGH);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(LED_B, LOW);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
#else
        while (1)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
#endif
    }
    /** END IMU **/

    /** MOTORS **/
    for (int i = 0; i < N_MOTORS && init; i++)
    {
        DC_AngPos[i] = 0;
        DC_AngPosLast[i] = 0;
        DC_AngVelEst[i] = 0;
        DC_AngVelEst_F[i] = new DiscreteLowPassFilter(0.4360604, 0.2819698, 0.2819698);
        DC_MotorsPosSensors[i] = new DCMotorPosSensorImpl(DC_ENC[i][0], DC_ENC[i][1], COUNTS2RAD);
        DC_Motors[i] = new DCMotorDriverImpl(i + 1);
        DC_MotorPlant[i] = new AbstractDCMotorPlant(DC_Motors[i], DC_MotorsPosSensors[i], 1 / DC_max_voltage, DC_max_voltage, DC_min_voltage);
        init = DC_Motors[i]->init();
        DC_IN[i] = new DiscreteLowPassFilter(0.96906992, 0.01546504, 0.01546504);

        CTRL_PID[i] = new ZPID(CTRL_Kp, CTRL_Ki, 0, 1, 0, CTRL_Ts, DC_min_voltage, DC_max_voltage, 0, 0, ZBlockMethod::Z_METHOD_TRAP, ZBlockMethod::Z_METHOD_BE);
    }
    if (!init)
    {
#ifdef DEBUG_CTRL
        Serial.println("[Control Task] Initialization of DC Motor Driver error");
        while (1)
        {
            digitalWrite(LED_B, HIGH);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(LED_B, LOW);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
#else
        while (1)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
#endif
    }
    /** END MOTORS **/


    /** BATTERY **/
    float CurrentVBatt = 0;
    VBatt = new DiscreteLowPassFilter(0.92442789, 0.03778605, 0.03778605);
    CVSensor = new INA219(INA219_I2C_ADDRESS, INA219_SHUNT_RESISTOR);
    init = CVSensor->begin();
    if (!init)
    {
#ifdef DEBUG_CTRL
        Serial.println("[Control Task] Initialization of DFRobot INA219 (Current and Voltage Sensor) error");
        while (1)
        {
            digitalWrite(LED_B, HIGH);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(LED_B, LOW);   
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
#else
        while (1)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
#endif
    }

    CVSensor->read();
    CurrentVBatt = CVSensor->busVoltage;
#ifdef DEBUG_CTRL
        Serial.print("[Control Task] Current Battery Voltage: ");
        Serial.print(CurrentVBatt);
        Serial.println();
#endif
    while(CurrentVBatt < VBatt_ON) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        CVSensor->read();
        CurrentVBatt = CVSensor->busVoltage;
        digitalWrite(LED_G, HIGH);   
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        CVSensor->read();
        CurrentVBatt = CVSensor->busVoltage;
        digitalWrite(LED_G, LOW);
#ifdef DEBUG_CTRL
        Serial.print("[Control Task] Current Battery Voltage: ");
        Serial.print(CurrentVBatt);
        Serial.println();
#endif
    }
    VBatt->initial_u(CurrentVBatt);
    /** END BATTERY **/

    int cont = 500;
    int cont_max = 500;
    float REF = 0;
    bool toggle = false;
    bool toggle_XY = false;
    bool toggle_X = false;
    bool toggle_Y = false;

    // CALIBRATE MOTOR WITH CURRENT VOLTAGE
    for (int i = 0; i < N_MOTORS; i++) DC_MotorPlant[i]->setGain(1/CurrentVBatt);

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, CTRL_TASK_CYCLE_RATE_MS);

        /** BATTERY **/
        CVSensor->read();
        CurrentVBatt = CVSensor->busVoltage;
        CurrentVBatt = VBatt->step(CurrentVBatt);
        /** END BATTERY **/

        /** IMU **/
        bno.getEvent(&event);               // Get sensor information
        Angle = (float)event.orientation.x; // extracting new angle

        // Unwrap angle
        AngleDiff = LastAngle - Angle;
        LastAngle = Angle;
        if (abs(AngleDiff) > AngleTolerance)
        {
            if (AngleDiff > 0)
                AngleCounts += 1.0f;
            else
                AngleCounts -= 1.0f;
        }
        // USEFUL ANGLE OUTPUT
        AngleOutput = (Angle + AngleCounts * Angle360DEG) * AngleDEG2RAD;
        /** END IMU **/

        /** MOTORS **/
        // ESTIMATING VELOCITY
        for (int i = 0; i < N_MOTORS; i++)
        {
            DC_AngPos[i] = DC_MotorPlant[i]->getPosition();
            DC_AngVelEst[i] = (DC_AngPos[i] - DC_AngPosLast[i])/(2.0f*ctrl_task_period_s);
            DC_AngVelEst[i] = -1*DC_AngVelEst_F[i]->step(DC_AngVelEst[i]); // FILTER
            DC_AngPosLast[i] = DC_AngPos[i];
        }
        // SETTING VOLTAGE GAIN
        for (int i = 0; i < N_MOTORS; i++) DC_MotorPlant[i]->setGain(1/CurrentVBatt);
        // GETTING AND FILTERING INPUT
        if(cont == cont_max){
            if(toggle){
                if(toggle_XY){
                    if(toggle_X){
                        REF_vX = 0.1;
                        toggle_X = false;
                    } else {
                        REF_vX = -0.1;
                        toggle_X = true;
                    }
                    toggle_XY = false;
                } else {
                    if(toggle_Y){
                        REF_vY = 0.1;
                        toggle_Y = false;
                    } else {
                        REF_vY = -0.1;
                        toggle_Y = true;
                    }
                    toggle_XY = true;
                }
                toggle = false;
            } else {
                REF_vX = 0;
                REF_vY = 0;
                toggle = true;
            }
            cont = 0;
        }
        cont++;
        // Read and load references
        for (int i = 0; i < N_MOTORS; i++) {
            DC_MOTOR_REF[i] = (1.0f/ROBOT_R) * (IKMixer[i][0] * REF_vX + IKMixer[i][1] * REF_vY + IKMixer[i][2] * REF_wZ);
        }
        // Filter references
        for (int i = 0; i < N_MOTORS; i++) DC_MOTOR_REF[i] = DC_IN[i]->step(DC_MOTOR_REF[i]);

        // Calculate the control action with controller
        for (int i = 0; i < N_MOTORS; i++) DC_MOTOR_U[i] = CTRL_PID[i]->update(DC_MOTOR_REF[i], DC_AngVelEst[i]);


        // Set control action
        for (int i = 0; i < N_MOTORS; i++) DC_MotorPlant[i]->setVoltage(DC_MOTOR_U[i]);

#ifdef DEBUG_CTRL_PID_MOTORS
        for (int i = 0; i < N_MOTORS; i++) {
            Serial.print(DC_MOTOR_REF[i]);
            Serial.print(",");
            Serial.print(DC_AngVelEst[i]);
            Serial.print(",");
        }
        // Serial.print(cont);
        // Serial.print(",");
        Serial.print(CurrentVBatt/4);
        Serial.print(",");
        Serial.print(AngleOutput);
        Serial.println();
#endif
    }
    // Will never get here!
}