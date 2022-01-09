/**
 * @file main.cpp
 * @author Rafael Carbonell Lázaro (racarla96 @ gmail.com)
 * @brief Validation test for EKFRicardo
 * @version 0.1
 * @date 23-12-2021
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <Eigen/Dense>
using namespace Eigen;

#include <iostream>
using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#define NOMBRE_ARCHIVO "filter_in.csv"
using namespace std;

#include <EKFRicardo.hpp>

int main()
{

    EKFRicardo EKF = EKFRicardo();
    EKF.setPosFB(-1, -1, 3, -1, 5, 5, -1, 5);

    state _state;

    // https://parzibyte.me/blog/2021/04/16/leer-csv-cpp/
    string str_vx, str_vy, str_wz, str_theta, str_d1, str_d2, str_d3, str_d4;
    float vx, vy, wz, theta, d1, d2, d3, d4;
    ifstream archivo(NOMBRE_ARCHIVO);
    string linea;
    char delimitador = ',';
    // Leemos la primer línea para descartarla, pues es el encabezado
    // getline(archivo, linea);ç

    bool first_line = false;
    int line = 0;
    int next_line = 10;
    int inc_line = 10;
    // Leemos todas las líneas
    while (getline(archivo, linea))
    {
        stringstream stream(linea); // Convertir la cadena a un stream
        // Extraer todos los valores de esa fila
        getline(stream, str_vx, delimitador);
        getline(stream, str_vy, delimitador);
        getline(stream, str_wz, delimitador);
        getline(stream, str_theta, delimitador);
        getline(stream, str_d1, delimitador);
        getline(stream, str_d2, delimitador);
        getline(stream, str_d3, delimitador);
        getline(stream, str_d4, delimitador);

        vx = std::stof(str_vx);
        vy = std::stof(str_vy);
        wz = std::stof(str_wz);
        theta = std::stof(str_theta);
        d1 = std::stof(str_d1);
        d2 = std::stof(str_d2);
        d3 = std::stof(str_d3);
        d4 = std::stof(str_d4);

        if (first_line)
        {
            if (line == next_line)
            {
                // // Imprimir
                // cout << "==================" << endl;
                // cout << " Vx: " << std::fixed << std::setprecision(2) << vx;
                // cout << " Vy: " << std::fixed << std::setprecision(2) << vy;
                // cout << " Wz: " << std::fixed << std::setprecision(2) << wz;
                // cout << " Theta: " << std::fixed << std::setprecision(2) << theta;
                // cout << " D1: " << std::fixed << std::setprecision(2) << d1;
                // cout << " D2: " << std::fixed << std::setprecision(2) << d2;
                // cout << " D3: " << std::fixed << std::setprecision(2) << d3;
                // cout << " D4: " << std::fixed << std::setprecision(2) << d4;
                // cout << endl;

                _state = EKF.update(vx, vy, wz, theta, d1, d2, d3, d4);

                next_line += inc_line;
            }
            else
            {
                // // Imprimir
                // cout << "==================" << endl;
                // cout << " Vx: " << std::fixed << std::setprecision(2) << vx;
                // cout << " Vy: " << std::fixed << std::setprecision(2) << vy;
                // cout << " Wz: " << std::fixed << std::setprecision(2) << wz;
                // cout << " Theta: " << std::fixed << std::setprecision(2) << theta;
                // cout << endl;

                _state = EKF.update(vx, vy, wz, theta);

            }
        }
        else
        {
            // // Imprimir
            // cout << "==================" << endl;
            // cout << "INIT VALUES FOR EKF" << endl;
            // cout << " Vx: " << std::fixed << std::setprecision(2) << vx;
            // cout << " Vy: " << std::fixed << std::setprecision(2) << vy;
            // cout << " Wz: " << std::fixed << std::setprecision(2) << wz;
            // cout << " Theta: " << std::fixed << std::setprecision(2) << theta;
            // cout << " D1: " << std::fixed << std::setprecision(2) << d1;
            // cout << " D2: " << std::fixed << std::setprecision(2) << d2;
            // cout << " D3: " << std::fixed << std::setprecision(2) << d3;
            // cout << " D4: " << std::fixed << std::setprecision(2) << d4;
            // cout << endl;

            _state = EKF.init(0, 0, 0);

            first_line = true;
        }
        line++;


        // cout << "==================" << endl;
        // cout << " X: " << std::fixed << std::setprecision(2) << _state.X;
        // cout << " Y: " << std::fixed << std::setprecision(2) << _state.Y;
        // cout << " Theta: " << std::fixed << std::setprecision(2) << _state.Theta;
        // cout << " Vx: " << std::fixed << std::setprecision(2) << _state.Vx;
        // cout << " Vy: " << std::fixed << std::setprecision(2) << _state.Vy;
        // cout << " Wz: " << std::fixed << std::setprecision(2) << _state.Wtheta;
        // cout << endl;

        cout << _state.X << ",";
        cout << _state.Y << ",";
        cout << _state.Theta << ",";
        cout << _state.Vx << ",";
        cout << _state.Vy << ",";
        cout << _state.Wtheta;
        cout << endl;

    }
    archivo.close();

    return 0;
}