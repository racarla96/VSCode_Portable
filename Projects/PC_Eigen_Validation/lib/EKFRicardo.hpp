/**
 * @file EKF_Ricardo.hpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief
 * @version 0.1
 * @date 22-12-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <Eigen/Dense>
using namespace Eigen;

#define N_ESTADOS 14
typedef Matrix<float, N_ESTADOS, N_ESTADOS> Matrix14x14f;
typedef Matrix<float, N_ESTADOS, 1> Matrix14x1f; 

#define N_MEASUREMENTS 8
typedef Matrix<float, N_MEASUREMENTS, N_MEASUREMENTS> Matrix8x8f;
typedef Matrix<float, N_MEASUREMENTS, 1> Matrix8x1f;

#define N3 3
#define N6 6
typedef Matrix<float, N3, N3> Matrix3x3f;
typedef Matrix<float, N6, N3> Matrix6x3f;
typedef Matrix<float, N_MEASUREMENTS, N3> Matrix8x3f;
typedef Matrix<float, N_MEASUREMENTS, N6> Matrix8x6f;
typedef Matrix<float, N6, N6> Matrix6x6f;
typedef Matrix<float, N_ESTADOS, N3> Matrix14x3f;
typedef Matrix<float, N_MEASUREMENTS, N_ESTADOS> Matrix8x14f;
typedef Matrix<float, N_ESTADOS, N_MEASUREMENTS> Matrix14x8f;


struct state
{
    float X;
    float Y;
    float Theta;
    float Vx;
    float Vy;
    float Wtheta;
};

class EKFRicardo
{
public:
    EKFRicardo(){};
    void setPosFB(float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4);
    state init(float x, float y, float theta); // Set initial position
    state update(float Vx, float Vy, float Wz, float Theta);
    state update(float Vx, float Vy, float Wz, float Theta, float d1, float d2, float d3, float d4);

private:
    state update();
    state internal_ret;

    const float _Ts = 0.1f;
    const float _Ts2 = (_Ts*_Ts)/2.0f;

    // Posición de las balizas fijas del mapa
    float _FBX1;
    float _FBY1;
    float _FBX2;
    float _FBY2;
    float _FBX3;
    float _FBY3;
    float _FBX4;
    float _FBY4;

    float _D1;
    float _D2;
    float _D3;
    float _D4;

    float _x;
    float _y;
    float _theta;
    float _vx;
    float _vy;
    float _wz;
    float _nu1;
    float _nu2;
    float _nu3;
    float _nu4;
    float _nu5;
    float _nu6;
    float _nu7;
    float _nu8;

    Matrix14x1f X;
    Matrix14x14f P;

    Matrix3x3f Q;
    Matrix8x8f R;

    Matrix8x1f z;
    Matrix8x8f D;
    Matrix8x8f ID;

    Matrix6x3f HkSR;
    Matrix14x3f Hk;

    Matrix8x6f dgkSR;
    Matrix8x14f dgk;

    Matrix6x6f dfkSR;
    Matrix14x14f dfk;

    Matrix14x8f K;

    Matrix8x1f gkSR;
    Matrix8x1f gk;

    Matrix8x8f Aux1;
    Matrix8x1f Aux2;
};