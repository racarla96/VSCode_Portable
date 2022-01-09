#include <EKFRicardo.hpp>

void EKFRicardo::setPosFB(float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4)
{
    _FBX1 = X1;
    _FBY1 = Y1;
    _FBX2 = X2;
    _FBY2 = Y2;
    _FBX3 = X3;
    _FBY3 = Y3;
    _FBX4 = X4;
    _FBY4 = Y4;
}

state EKFRicardo::init(float x, float y, float theta)
{
    X.setZero();
    P.setZero();

    Q.setZero();
    R.setZero();

    z.setZero();
    D.setZero();
    ID.setZero();

    HkSR.setZero();
    Hk.setZero();

    dgkSR.setZero();
    dgk.setZero();

    dfkSR.setZero();
    dfk.setZero();

    K.setZero();

    gkSR.setZero();
    gk.setZero();

    Aux1.setZero();
    Aux2.setZero();

    X(0, 0) = x;
    X(1, 0) = y;
    X(2, 0) = theta;

    Q.diagonal() << 30, 30, 30;
    R.diagonal() << 1e-1, 1e-1, 1e-1, 1e-5, 1e-5, 1e-5, 1e-5, 1e-5;

    Hk(2, 2) = _Ts2;
    Hk(3, 0) = _Ts;
    Hk(4, 1) = _Ts;
    Hk(5, 2) = _Ts;

    dgkSR.topRightCorner(N3, N3) = MatrixXf::Identity(N3, N3);
    dgkSR(3, 2) = 1.0f;

    dfkSR.setIdentity();
    dfkSR(2, 5) = _Ts;

    internal_ret.X = X(0, 0);
    internal_ret.Y = X(1, 0);
    internal_ret.Theta = X(2, 0);
    internal_ret.Vx = X(3, 0);
    internal_ret.Vy = X(4, 0);
    internal_ret.Wtheta = X(5, 0);
    return internal_ret;
}

state EKFRicardo::update(float Vx, float Vy, float Wz, float Theta)
{
    D.diagonal() << 1, 1, 1, 1, 0, 0, 0, 0;
    ID = MatrixXf::Identity(N_MEASUREMENTS, N_MEASUREMENTS)-D;
    z(0, 0) = Vx;
    z(1, 0) = Vy;
    z(2, 0) = Wz;
    z(3, 0) = Theta;

    return update();
}

state EKFRicardo::update(float Vx, float Vy, float Wz, float Theta, float d1, float d2, float d3, float d4)
{
    D.diagonal() << 1, 1, 1, 1, 1, 1, 1, 1;
    ID = MatrixXf::Identity(N_MEASUREMENTS, N_MEASUREMENTS)-D;
    z(0, 0) = Vx;
    z(1, 0) = Vy;
    z(2, 0) = Wz;
    z(3, 0) = Theta;
    z(4, 0) = d1;
    z(5, 0) = d2;
    z(6, 0) = d3;
    z(7, 0) = d4;

    return update();
}

state EKFRicardo::update()
{
    _x = X(0, 0);
    _y = X(1, 0);
    _theta = X(2, 0);
    _vx = X(3, 0);
    _vy = X(4, 0);
    _wz = X(5, 0);
    _nu1 = X(6, 0);
    _nu2 = X(7, 0);
    _nu3 = X(8, 0);
    _nu4 = X(9, 0);
    _nu5 = X(10, 0);
    _nu6 = X(11, 0);
    _nu7 = X(12, 0);
    _nu8 = X(13, 0);

    Hk(0, 0) = (_Ts2 * cosf(_theta));
    Hk(0, 1) = -(_Ts2 * sinf(_theta));
    Hk(1, 0) = (_Ts2 * sinf(_theta));
    Hk(1, 1) = (_Ts2 * cosf(_theta));

    _D1 = sqrtf((_x - _FBX1) * (_x - _FBX1) + (_y - _FBY1) * (_y - _FBY1));
    _D2 = sqrtf((_x - _FBX2) * (_x - _FBX2) + (_y - _FBY2) * (_y - _FBY2));
    _D3 = sqrtf((_x - _FBX3) * (_x - _FBX3) + (_y - _FBY3) * (_y - _FBY3));
    _D4 = sqrtf((_x - _FBX4) * (_x - _FBX4) + (_y - _FBY4) * (_y - _FBY4));

    dgkSR(4, 0) = (_x - _FBX1) / _D1;
    dgkSR(5, 0) = (_x - _FBX2) / _D2;
    dgkSR(6, 0) = (_x - _FBX3) / _D3;
    dgkSR(7, 0) = (_x - _FBX4) / _D4;
    dgkSR(4, 1) = (_y - _FBY1) / _D1;
    dgkSR(5, 1) = (_y - _FBY2) / _D2;
    dgkSR(6, 1) = (_y - _FBY3) / _D3;
    dgkSR(7, 1) = (_y - _FBY4) / _D4;

    dfkSR(0, 2) = -_Ts*(sinf(_theta)*_vx+cosf(_theta)*_vy);
    dfkSR(0, 3) = _Ts*(cosf(_theta));
    dfkSR(0, 4) = -_Ts*(sinf(_theta));
    dfkSR(1, 2) = _Ts*(cosf(_theta)*_vx+sinf(_theta)*_vy);
    dfkSR(1, 3) = _Ts*(sinf(_theta));
    dfkSR(1, 4) = _Ts*cosf((_theta));

    dfk.topLeftCorner(dfkSR.rows(), dfkSR.cols()) = dfkSR;
    dfk.bottomLeftCorner(dgkSR.rows(), dgkSR.cols()) = D*dgkSR;
    dfk.bottomRightCorner(N_MEASUREMENTS, N_MEASUREMENTS) = ID;

    dgk.topLeftCorner(dgkSR.rows(), dgkSR.cols()) = D*dgkSR;
    dgk.topRightCorner(N_MEASUREMENTS, N_MEASUREMENTS) = ID;

    P = dfk*P*dfk.transpose() + Hk*Q*Hk.transpose();
    Aux1 = dgk*P*dgk.transpose() + R;
    K = P*dgk.transpose()*Aux1.inverse()*D;
    P = P - K*dgk*P;

    // _D1 = sqrtf((_x - _FBX1) * (_x - _FBX1) + (_y - _FBY1) * (_y - _FBY1));
    // _D2 = sqrtf((_x - _FBX2) * (_x - _FBX2) + (_y - _FBY2) * (_y - _FBY2));
    // _D3 = sqrtf((_x - _FBX3) * (_x - _FBX3) + (_y - _FBY3) * (_y - _FBY3));
    // _D4 = sqrtf((_x - _FBX4) * (_x - _FBX4) + (_y - _FBY4) * (_y - _FBY4));

    X(0, 0) = _x+_Ts*(_vx*cosf(_theta)-_vy*sinf(_theta));
    X(1, 0) = _y+_Ts*(_vx*sinf(_theta)-_vy*cosf(_theta));
    X(2, 0) = _theta+_Ts*_wz;
    X(3, 0) = _vx;
    X(4, 0) = _vy;
    X(5, 0) = _wz;
    X(6, 0) = D(0, 0) * _vx + ID(0, 0) * _nu1;
    X(7, 0) = D(1, 1) * _vy + ID(1, 1) * _nu2;
    X(8, 0) = D(2, 2) * _wz + ID(2, 2) * _nu3;
    X(9, 0) = D(3, 3) * _theta + ID(3, 3) * _nu4;
    X(10, 0) = D(4, 4) * _D1 + ID(4, 4) * _nu5;
    X(11, 0) = D(5, 5) * _D2 + ID(5, 5) * _nu6;
    X(12, 0) = D(6, 6) * _D3 + ID(6, 6) * _nu7;
    X(13, 0) = D(7, 7) * _D4 + ID(7, 7) * _nu8;

    _x = X(0, 0);
    _y = X(1, 0);
    _theta = X(2, 0);
    _vx = X(3, 0);
    _vy = X(4, 0);
    _wz = X(5, 0);
    _nu1 = X(6, 0);
    _nu2 = X(7, 0);
    _nu3 = X(8, 0);
    _nu4 = X(9, 0);
    _nu5 = X(10, 0);
    _nu6 = X(11, 0);
    _nu7 = X(12, 0);
    _nu8 = X(13, 0);

    _D1 = sqrtf((_x - _FBX1) * (_x - _FBX1) + (_y - _FBY1) * (_y - _FBY1));
    _D2 = sqrtf((_x - _FBX2) * (_x - _FBX2) + (_y - _FBY2) * (_y - _FBY2));
    _D3 = sqrtf((_x - _FBX3) * (_x - _FBX3) + (_y - _FBY3) * (_y - _FBY3));
    _D4 = sqrtf((_x - _FBX4) * (_x - _FBX4) + (_y - _FBY4) * (_y - _FBY4));

    gkSR << _vx, _vy, _wz, _theta, _D1, _D2, _D3, _D4;
    Aux2 << _nu1, _nu2, _nu3, _nu4, _nu5, _nu6, _nu7, _nu8;
    gk = D*gkSR + ID*Aux2;

    X = X + K*(z - gk);

    internal_ret.X = X(0, 0);
    internal_ret.Y = X(1, 0);
    internal_ret.Theta = X(2, 0);
    internal_ret.Vx = X(3, 0);
    internal_ret.Vy = X(4, 0);
    internal_ret.Wtheta = X(5, 0);
    return internal_ret;
}