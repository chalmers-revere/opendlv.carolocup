//Authors: Pedram Shirinbolaghi and Love from CRF.
#include <Smartcar.h>
#define pin1 A0

GP2D120 rearLeftIR;
//Declare variables for filter
int value;

int lastY, Y, X;

  const float alpha = 0.75; 

void setup() {


  Serial.begin(9600);

  rearLeftIR.attach(pin1);

}

void loop() {
  

  //Digital noise filter
  Serial.print("Sensor value is ");
  X = float(rearLeftIR.getDistance());

  Y = (1-alpha)*lastY + alpha*X; 
  lastY =Y; 
  
  Serial.println(Y);
}
