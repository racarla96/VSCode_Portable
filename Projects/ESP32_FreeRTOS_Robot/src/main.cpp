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