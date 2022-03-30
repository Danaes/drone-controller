#include <Arduino.h>
#include "Adafruit_MMA8451.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_MCP4728.h>
#include <Wire.h>
#include "SPI.h"

// Objetos de comunicación con los sensores
Adafruit_MMA8451 mma = Adafruit_MMA8451();
Adafruit_MCP4728 mcp;
const int distanceSensor = A0;

// Variables de los sensores
int inputDistance = 0;
int outputHeight;
double inputAccelerometer[2];
uint16_t outputControl[] = {0, 4095, 0, 4095};
int outputDistance[] = {4095, 3042, 2048, 1024, 0};

// Métodos
void setUpAccelerometer();
void setUpDAC();
void setUpEngines();
void readAccelerometer();
void transferFunctionXYPlane();
void readDistanceSensor();
void sendDataToDAC();
 
void setup(void) {
  Serial.begin(9600);

  while (!Serial)
    delay(10);

  setUpAccelerometer();
  setUpDAC();

  setUpEngines();
}
 
void loop() {

  readAccelerometer();
  transferFunctionXYPlane();
  
  if(outputControl[0] == 2048 && outputControl[1] == 2048) readDistanceSensor();
  else outputControl[2] = 2048;

  sendDataToDAC();
 
  delay(500);
}

void setUpAccelerometer(){
  Serial.println("Adafruit MMA8451 test!");
  
  // Try to initialize!
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
 
  mma.setRange(MMA8451_RANGE_2_G);
}

void readAccelerometer(){
  mma.read();
  
  double X = mma.x;
  double Y = mma.y;
  double Z = mma.z;
  
  if(X >= 32768) X = -(65536 - X);
  if(Y >= 32768) Y = -(65536 - Y);
  if(Z >= 32768) Z = -(65536 - Z);
 
  X = X / 16384.0;
  Y = Y / 16384.0;
  Z = Z / 16384.0;

  double Rx = atan(Y / sqrt(X*X + Z*Z)) * 180.0/3.14;
  double Ry = atan(X / sqrt(Y*Y + Z*Z)) * 180.0/3.14;

  inputAccelerometer[0] = Rx;
  inputAccelerometer[1] = Ry;
  
}

void transferFunctionXYPlane(){

  double x = inputAccelerometer[0];
  double y = inputAccelerometer[1];

  // Serial.print("X: \t"); Serial.print(x);
  // Serial.print("Y: \t"); Serial.println(y); Serial.println();

  if( x < -10) outputControl[0] = 2048 * (-x / 50 + 1);
  else if( x >= -10 && x <= 30) outputControl[0] = 2048;
  else outputControl[0] = 2048 * (-x / 90 + 1);

  if( y < -5) outputControl[1] = 2048 * (y / 50 + 1);
  else if( y >= -5 && y <= 15) outputControl[1] = 2048;
  else outputControl[1] = 2048 * (y / 85 + 1);

  if(outputControl[0] < 0) outputControl[0] = 0;
  else if(outputControl[0] > 4096 ) outputControl[0] = 4096;

  if(outputControl[1] < 0) outputControl[1] = 0;
  else if(outputControl[1] > 4096 ) outputControl[1] = 4096;

  // String msgX, msgY;
  // if (outputControl[0] < 2048) msgX = "Derecha:\t";
  // else if (outputControl[0] > 2048) msgX = "Izquierda:\t";
  // else msgX = "Quieto\t";
  // if (outputControl[1] < 2048) msgY = "\tAvanzo:\t";
  // else if (outputControl[1] > 2048) msgY = "\tRetrocedo:\t";
  // else msgY = "\tQuieto\t";
  // Serial.print(msgX); Serial.print(outputControl[0]); 
  // Serial.print(msgY); Serial.println(outputControl[1]);

}

void readDistanceSensor(){

  inputDistance = analogRead(distanceSensor);
  int outputValue = map(inputDistance, 180, 800, 0, 256);

  // Serial.print("Valor: "); Serial.print(inputDistance);
  // Serial.print("\t Valor: "); Serial.println(outputValue);

  outputControl[2] = outputDistance[outputValue / 51];
  // Serial.print("\t Valor Final: "); Serial.println(outputControl[2]);

}

void setUpDAC() {
  Serial.println("Adafruit MCP4728 test!");
 
  // Try to initialize!
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }

  delay(100);
 
}

void sendDataToDAC() {

  Serial.print("\t Valor A: "); Serial.println(outputControl[0]);
  Serial.print("\t Valor B: "); Serial.println(outputControl[1]);
  Serial.print("\t Valor C: "); Serial.println(outputControl[2]);
  Serial.print("\t Valor D: "); Serial.println(outputControl[3]);Serial.println();

  mcp.setChannelValue(MCP4728_CHANNEL_A, outputControl[0]); // Izquierda o Derecha
  delay(10);
  mcp.setChannelValue(MCP4728_CHANNEL_B, outputControl[1]); // Avanzar o retroceder
  delay(10);
  mcp.setChannelValue(MCP4728_CHANNEL_C, outputControl[2]); // Altura
  delay(10);
  mcp.setChannelValue(MCP4728_CHANNEL_D, outputControl[3]);

}

void setUpEngines() {
  
  delay(3500);
  
  sendDataToDAC();

  delay(1500);

  outputControl[0] = 2048;
  outputControl[1] = 2048;
  outputControl[2] = 4095;
  outputControl[3] = 2048;

  sendDataToDAC();

  delay(4000);
  
  outputControl[0] = 2048;
  outputControl[1] = 2048;
  outputControl[2] = 2048;
  outputControl[3] = 2048;

}