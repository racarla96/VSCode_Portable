#include <Arduino.h>

#include <ArduinoEigen.h>
using namespace Eigen;

void print_mtxf(const Eigen::MatrixXf& X);

void setup() {
  // Initialize the Serial in a common task
  Serial.begin(115200);

  vTaskDelay(10000 / portTICK_PERIOD_MS); // prevents usb driver crash on startup, do not omit this
  while (!Serial){} // Wait for Serial terminal to open port before starting program

  MatrixXf A(3, 3);
  A << 1, 2, 3,
      4, 5, 6,
      7, 8, 9;

  MatrixXf D(3,3);
  D.setZero();
  D.diagonal() << 1, 0, 1;

  MatrixXf R(3, 3);
  R = (MatrixXf::Identity(3,3) - D) * A;

  print_mtxf(A);
  print_mtxf(D);
  print_mtxf(R);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// PRINT MATRIX (float type)
// By: randomvibe
//-----------------------------
void print_mtxf(const Eigen::MatrixXf& X)  
{
   int i, j, nrow, ncol;
   
   nrow = X.rows();
   ncol = X.cols();

   Serial.print("nrow: "); Serial.println(nrow);
   Serial.print("ncol: "); Serial.println(ncol);      
   Serial.println();
   
   for (i=0; i<nrow; i++)
   {
       for (j=0; j<ncol; j++)
       {
           Serial.print(X(i,j), 6);   // print 6 decimal places
           Serial.print(", ");
       }
       Serial.println();
   }
   Serial.println();
}