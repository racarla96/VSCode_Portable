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
#define NSTATES_EXT 14
#define NSTATES 6
#define NSTATES_RET 3 // RETAIL - X, Y, Z
#define NMESURES 8

// STATE POSITION
#define SP_X 0
#define SP_Y 1
#define SP_O 2
#define SP_VX 3
#define SP_VY 4
#define SP_WZ 5
#define SP_X 0
#define SP_X 0
#define SP_X 0

void MotorsControlTask(void *pvParameters)
{
    bool init = true; // All is initialized correctly

    // ROBOT
    const float ROBOT_L = 0.15f;
    const float ROBOT_W = 0.15f;
    const float ROBOT_ILW = 1.0f/(ROBOT_L+ROBOT_W);
    const float ROBOT_R = 0.05f;

    // Direct and Inverse Kinematics
    MatrixXf IKMixer(N_MOTORS, 3);
    IKMixer <<  1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f;
    MatrixXf DKMixer(3, N_MOTORS);
    DKMixer << 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f*ROBOT_ILW, -1.0f*ROBOT_ILW, 1.0f*ROBOT_ILW, -1.0f*ROBOT_ILW;

    // // EKF
    // const float Ts = CTRL_Ts;

    // MatrixXd IKMeasures(3, 1); // Vumedida, Vvmedida, Wwmedida
    // IKMeasures << MatrixXd::Zero(3, 1);
    // MatrixXd WMeasures(N_MOTORS, 1); // W1, W2, W3, W4
    // WMeasures << MatrixXd::Zero(N_MOTORS, 1);

    // float x, y, phi, Vu, Vv, Wz = 0.0f;
    // float nu1, nu2, nu3, nu4, nu5, nu6, nu7, nu8 = 0.0f;

    // bool haveDistances = false;
    // float DBaliza1, DBaliza2, DBaliza3, DBaliza4 = 0.0f;

    // MatrixXd X(NSTATES_EXT, 1);
    // X << MatrixXd::Zero(NSTATES_EXT, 1);

    // MatrixXd P(NSTATES_EXT, NSTATES_EXT);
    // P << MatrixXd::Zero(NSTATES_EXT, NSTATES_EXT);

    // MatrixXd K(NSTATES_EXT, 1);
    // K << MatrixXd::Zero(NSTATES_EXT, 1);

    // MatrixXd z(NMESURES, 1);
    // z << MatrixXd::Zero(NMESURES, 1);

    // MatrixXd Q(NSTATES_RET, NSTATES_RET);
    // Q.diagonal() << 30, 30, 30;

    // MatrixXd R(NMESURES, NMESURES);
    // R.diagonal() << 1e-1, 1e-1, 1e-1, 1e-5, 1e-5, 1e-5, 1e-5, 1e-5;
    // MatrixXd Temp(NMESURES, NMESURES);
    // Temp << MatrixXd::Zero(NMESURES, NMESURES);

    // float diagVx, diagVy, diagWz, diagAng, diagBeacons = 1.0f; // delta1, delta2, delta3, delta4, (delta5, delta6, delta7, delta8)
    // MatrixXd D(NMESURES, NMESURES);
    // D << MatrixXd::Zero(NMESURES, NMESURES);
    // MatrixXd ID(NMESURES, NMESURES);
    // ID << MatrixXd::Zero(NMESURES, NMESURES);
    // MatrixXd nu(NMESURES, 1);
    // nu << MatrixXd::Zero(NMESURES, 1);

    // MatrixXd Hk(NSTATES_EXT, NSTATES_RET); // 14x3
    // Hk << MatrixXd::Zero(NSTATES_EXT, NSTATES_RET);
    // MatrixXd dfk(NSTATES_EXT, NSTATES_EXT); // 14x14
    // dfk << MatrixXd::Zero(NSTATES_EXT, NSTATES_EXT);
    // MatrixXd dgk(NMESURES, NSTATES_EXT); // 8x14
    // dgk << MatrixXd::Zero(NMESURES, NSTATES_EXT);

    // float Vu, Vv, Wz, phi;
    // const float PosBaliza1x = 0.0f;
    // const float PosBaliza1y = 0.0f;
    // const float PosBaliza2x = 0.0f;
    // const float PosBaliza2y = 0.0f;
    // const float PosBaliza3x = 0.0f;
    // const float PosBaliza3y = 0.0f;
    // const float PosBaliza4x = 0.0f;
    // const float PosBaliza4y = 0.0f;
    // MatrixXd gkSR(NMESURES, 1); // 8x1
    // gkSR << MatrixXd::Zero(NMESURES, 1);
    // MatrixXd gk(NMESURES, 1); // 8x1
    // gk << MatrixXd::Zero(NMESURES, 1);

    UDP_PACKET_REFERENCES REFERENCES;

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
    while (CurrentVBatt < VBatt_ON)
    {
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
    for (int i = 0; i < N_MOTORS; i++)
        DC_MotorPlant[i]->setGain(1 / CurrentVBatt);

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, CTRL_TASK_CYCLE_RATE_MS);

        // MEDIDAS
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
            DC_AngVelEst[i] = (DC_AngPos[i] - DC_AngPosLast[i]) / (2.0f * ctrl_task_period_s);
            DC_AngVelEst[i] = -1 * DC_AngVelEst_F[i]->step(DC_AngVelEst[i]); // FILTER
            DC_AngPosLast[i] = DC_AngPos[i];
        }

        // SETTING VOLTAGE GAIN
        for (int i = 0; i < N_MOTORS; i++)
            DC_MotorPlant[i]->setGain(1 / CurrentVBatt);
        // END MEDIDAS

        // EKF
        // Delta Matrix
        // Si hay medidas de balizas - diagBeacons = 1.0f;
        // Si no hay medidas de balizas - diagBeacons = 0.0f;
        // if(haveDistances) {
        //     diagBeacons = 1.0f;
        //     D.diagonal() << diagVx, diagVy, diagWz, diagAng, diagBeacons, diagBeacons, diagBeacons, diagBeacons;
        //     ID.diagonal() << 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;

        // } else {
        //     diagBeacons = 0.0f;
        //     D.diagonal() << diagVx, diagVy, diagWz, diagAng, diagBeacons, diagBeacons, diagBeacons, diagBeacons;
        //     ID.diagonal() << 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f;
        // }


        // for (int i = 0; i < N_MOTORS; i++)
        //     WMeasures(i) = DC_AngVelEst[i];

        // IKMeasures = ((ROBOT_R / 4.0f) * DKMixer) * WMeasures;
        // z(0) = IKMeasures(0); z(1) = IKMeasures(1); z(2) = IKMeasures(2);
        // z(3) = AngleOutput; 
        // z(4) = DBaliza1; z(5) = DBaliza2; z(6) = DBaliza3; z(7) = DBaliza4;

        // x = X(0); y = X(1); phi = X(2); 
        // Vu = X(3); Vv = X(4); Wz = X(5);
        // nu1 = X(6); nu2 = X(7); nu3 = X(8); nu4 = X(9); 
        // nu5 = X(10); nu6 = X(11); nu7 = X(12); nu8 = X(13);

        // Hk(0,0) = ((Ts*Ts)/2.0f)*cos(phi);
        // Hk(0,1) = -((Ts*Ts)/2.0f)*sin(phi);
        // Hk(1,0) = ((Ts*Ts)/2.0f)*sin(phi);
        // Hk(1,1) = ((Ts*Ts)/2.0f)*cos(phi);
        // Hk(2,2) = ((Ts*Ts)/2.0f);
        // Hk(3,0) = Ts;
        // Hk(4,1) = Ts;
        // Hk(5,2) = Ts;

        // dgkSR = [
        //     0, 0, 0, 1, 0, 0;
        //     0, 0, 0, 0, 1, 0;
        //     0, 0, 0, 0, 0, 1;
        //     0, 0, 1, 0, 0, 0;
        //     (x-PosBaliza1x)/sqrt((x-PosBaliza1x)^2 + (y-PosBaliza1y)^2), (y-PosBaliza1y)/sqrt((x-PosBaliza1x)^2 + (y-PosBaliza1y)^2), 0, 0, 0, 0;
        //     (x-PosBaliza2x)/sqrt((x-PosBaliza2x)^2 + (y-PosBaliza2y)^2), (y-PosBaliza2y)/sqrt((x-PosBaliza2x)^2 + (y-PosBaliza2y)^2), 0, 0, 0, 0;
        //     (x-PosBaliza3x)/sqrt((x-PosBaliza3x)^2 + (y-PosBaliza3y)^2), (y-PosBaliza3y)/sqrt((x-PosBaliza3x)^2 + (y-PosBaliza3y)^2), 0, 0, 0, 0;
        //     (x-PosBaliza4x)/sqrt((x-PosBaliza4x)^2 + (y-PosBaliza4y)^2), (y-PosBaliza4y)/sqrt((x-PosBaliza4x)^2 + (y-PosBaliza4y)^2), 0, 0, 0, 0];    

        // dfkSR = [1, 0, -Ts*(sin(phi)*Vu+cos(phi)*Vv), Ts*cos(phi), -Ts*sin(phi), 0;
        //     0, 1, Ts*(cos(phi)*Vu-sin(phi)*Vv), Ts*sin(phi), Ts*cos(phi),0;
        //     0, 0, 1, 0, 0, Ts; 
        //     0, 0, 0, 1, 0, 0;
        //     0, 0, 0, 0, 1, 0; 
        //     0, 0, 0, 0, 0, 1];
        // dfk = [dfkSR, zeros(6,8); Delta*dgkSR, eye(8)-Delta];
        // dgk = [Delta*dgkSR, eye(8)-Delta];
        
        // dgk(0, 0) = 

        // P = dfk*P*dfk.transpose() + Hk*Q*Hk.transpose();

        // Temp = dgk*P*dgk.transpose() + R;
        // K = P*dgk.transpose() * Temp.inverse() * D;

        // P = P - K*dgk*P;

        // X(0) = x+Ts*(Vu*cos(phi)-Vv*sin(phi)); 
        // X(1) = y+Ts*(Vu*sin(phi)-Vv*cos(phi)); 
        // X(2) = phi+Ts*Wz; 
        // X(3) = Vu; 
        // X(4) = Vv; 
        // X(5) = Wz;
        // X(6) = D(0) * Vu + ID(0) * nu1; 
        // X(7) = D(1) * Vv + ID(1) * nu2; 
        // X(8) = D(2) * Wz + ID(2) * nu3;
        // X(9) = D(3) * phi + ID(3) * nu4;
        // X(10) = D(4) * (sqrt((x-PosBaliza1x)*(x-PosBaliza1x) + (y-PosBaliza1y)*(y-PosBaliza1y))) + ID(4) * nu5;
        // X(11) = D(5) * (sqrt((x-PosBaliza2x)*(x-PosBaliza2x) + (y-PosBaliza2y)*(y-PosBaliza2y))) + ID(5) * nu6;
        // X(12) = D(6) * (sqrt((x-PosBaliza3x)*(x-PosBaliza3x) + (y-PosBaliza3y)*(y-PosBaliza3y))) + ID(6) * nu7;
        // X(13) = D(7) * (sqrt((x-PosBaliza4x)*(x-PosBaliza4x) + (y-PosBaliza4y)*(y-PosBaliza4y))) + ID(7) * nu8;

        // x = X(0); y = X(1); phi = X(2); 
        // Vu = X(3); Vv = X(4); Wz = X(5);
        // nu1 = X(6); nu2 = X(7); nu3 = X(8); nu4 = X(9); 
        // nu5 = X(10); nu6 = X(11); nu7 = X(12); nu8 = X(13);

        // gkSR(0) = Vu;
        // gkSR(1) = Vv; 
        // gkSR(2) = Wz; 
        // gkSR(3) = phi;
        // gkSR(4) = (sqrt((x-PosBaliza1x)*(x-PosBaliza1x) + (y-PosBaliza1y)*(y-PosBaliza1y)));
        // gkSR(5) = (sqrt((x-PosBaliza2x)*(x-PosBaliza2x) + (y-PosBaliza2y)*(y-PosBaliza2y)));
        // gkSR(6) = (sqrt((x-PosBaliza3x)*(x-PosBaliza3x) + (y-PosBaliza3y)*(y-PosBaliza3y)));
        // gkSR(7) = (sqrt((x-PosBaliza4x)*(x-PosBaliza4x) + (y-PosBaliza4y)*(y-PosBaliza4y)));

        // nu << nu1, nu2, nu3, nu4, nu5, nu6, nu7, nu8;
        // gk = D*gkSR + ID*nu;

        // X = X + K*(z - gk);
        // END EKF


        // GETTING AND FILTERING INPUT
        if( xSemaphoreTake( SEM_REFERENCES, ( TickType_t ) 0 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */
            REFERENCES = UDP_REFERENCES;

            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( SEM_REFERENCES );
        }

        // Read and load references
        for (int i = 0; i < N_MOTORS; i++)
        {
            DC_MOTOR_REF[i] = (1.0f / ROBOT_R) * (IKMixer(i, 0) * -REFERENCES.R_vX + IKMixer(i, 1) * -REFERENCES.R_vY + IKMixer(i, 2) * REFERENCES.R_wO);
        }
        // Filter references
        for (int i = 0; i < N_MOTORS; i++)
            DC_MOTOR_REF[i] = DC_IN[i]->step(DC_MOTOR_REF[i]);

        // Calculate the control action with controller
        for (int i = 0; i < N_MOTORS; i++)
            DC_MOTOR_U[i] = CTRL_PID[i]->update(DC_MOTOR_REF[i], DC_AngVelEst[i]);

        // Set control action
        for (int i = 0; i < N_MOTORS; i++)
            DC_MotorPlant[i]->setVoltage(DC_MOTOR_U[i]);

// #ifdef DEBUG_CTRL_PID_MOTORS
//         for (int i = 0; i < N_MOTORS; i++)
//         {
//             Serial.print(DC_MOTOR_REF[i]);
//             Serial.print(",");
//             Serial.print(DC_AngVelEst[i]);
//             Serial.print(",");
//         }
//         // Serial.print(cont);
//         // Serial.print(",");
//         Serial.print(CurrentVBatt / 4);
//         Serial.print(",");
//         Serial.print(AngleOutput);
//         Serial.println();
// #endif
    }
    // Will never get here!
}