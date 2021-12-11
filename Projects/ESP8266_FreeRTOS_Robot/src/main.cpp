#include <Arduino.h>

#include <SLHRobot.h>
#include <AbstractDCMotorDriver.h>
#include <DCMotorDriverImpl.h>

#define N_MOTORS 4
AbstractDCMotorDriver * AbstractMotors[N_MOTORS];


void setup() {
  bool init = true; // All is initialized correctly

  Serial.begin(115200);

  for(int i=0; i<N_MOTORS; i++) {
    AbstractMotors[i] = new DCMotorDriverImpl(i+1);
    init = AbstractMotors[i]->init();
    if(!init) break;
  }
  if(!init){
    Serial.println("Initialization DC Motor Driver error");
    while(1) {
      Serial.println("F");
      delay(1000);
    }
  } 

}

void loop() {
  for(int i=0; i<N_MOTORS; i++) {
    AbstractMotors[i]->setValue(0);
  }
  delay(1000);

  for(int i=0; i<N_MOTORS; i++) {
    AbstractMotors[i]->setValue(0.1);
  }  
  delay(1000);
}

// #include <Arduino.h>
// #include <Wire.h>

// void setup()
// {
//   Wire.begin();

//   Serial.begin(115200);
//   while (!Serial);             // Leonardo: wait for serial monitor
//   Serial.println("\nI2C Scanner Wire");
// }


// void loop()
// {
//   byte error, address;
//   int nDevices;

//   Serial.println("Scanning...");

//   nDevices = 0;
//   for(address = 1; address < 127; address++ ) 
//   {
//     // The i2c_scanner uses the return value of
//     // the Write.endTransmisstion to see if
//     // a device did acknowledge to the address.
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");

//       nDevices++;
//     }
//     else if (error==4) 
//     {
//       Serial.print("Unknown error at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");

//   delay(5000);           // wait 5 seconds for next scan
// }