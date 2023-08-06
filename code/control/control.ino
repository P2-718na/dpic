#include <AccelStepper.h>
#include <esp32-hal-adc.h>
#include "RunningAverage.h"

// Steps per revolution
#define STEPS 200


// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver
// 32 = step, 33=dir
AccelStepper stepper(1, 33, 32);


long int count = 0;
int phaseB;
void IRAM_ATTR myISR() {

  phaseB = analogRead(26);

  if (phaseB > 2500)
    --count;
  else
    ++count;
}


void setup() {
  Serial.begin(115200);

  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(20000);

  attachInterrupt(27, myISR, RISING);

  Serial.println("ciao");

}

long int dt = 0;
long int oldt = 0;
long int oldCount = 0;
int dcount = 0;
RunningAverage myRA(4);


int getAccel() {
  return (count % 600 - 300 - myRA.getAverage()*1000);;
}

void printStatus() {
    dt = millis() - oldt;
  dcount= count - oldCount;

  oldt = millis();
  oldCount = count;

  //Serial.print(count);
  //Serial.print(" ");
  myRA.add(float(dcount) / (float) dt * 100.);
  //Serial.print(getAccel());
  //Serial.print(" ");
  //Serial.println(myRA.getAverage());
}

void loop() {
  static const unsigned long REFRESH_INTERVAL = 5; // ms
	static unsigned long lastRefreshTime = 0;
	
	if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
	{
		lastRefreshTime = millis();
     printStatus();
	}

  if (abs(getAccel()) < 100) {
    stepper.setAcceleration(getAccel()*10000000);
    stepper.move(getAccel());
    stepper.run();
  }




  //stepper.move(20000);
  //stepper.runToPosition();
  //Serial.println(1);
  //stepper.move(-20000);
  //stepper.runToPosition();
  //Serial.println(0);
}
