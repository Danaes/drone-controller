#include "headers/i2c_sensors.h"

Adafruit_MMA8451 mma = Adafruit_MMA8451();
Adafruit_MCP4728 mcp;
AnalogSensors analogSensors;

uint16_t outputControl[] = {0, 4095, 0, 4095};
double inputAccelerometer[2];


void I2cSensors::setup(){
    pinMode(LED_BUILTIN,OUTPUT);
    setUpAccelerometer();
    setUpDAC();
    setUpEngines();
}

void I2cSensors::loop(){
    readAccelerometer();
    transferFunction();
    sendDataToDAC();
}

void I2cSensors::setUpAccelerometer(){
    Serial.println("Adafruit MMA8451 test!");

    // Try to initialize!
    digitalWrite(LED_BUILTIN,HIGH);

    if (! mma.begin()) {
        Serial.println("Couldnt start");
        while (1);
    }

    Serial.println("MMA8451 found!");

    mma.setRange(MMA8451_RANGE_8_G);
}

void I2cSensors::setUpDAC(){
    Serial.println("Adafruit MCP4728 test!");

    // Try to initialize!
    digitalWrite(LED_BUILTIN,HIGH);

    if (!mcp.begin()) {
        Serial.println("Failed to find MCP4728 chip");
        while (1) {
            delay(10);
        }
    }

    delay(100);
}

void I2cSensors::setUpEngines(){
    
    delay(3500);

    sendDataToDAC();

    delay(4000);

    outputControl[0] = 2048;
    outputControl[1] = 2048;
    outputControl[2] = 2048;
    outputControl[3] = 2048;

}

void I2cSensors::readAccelerometer(){
    digitalWrite(LED_BUILTIN,LOW);

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

void I2cSensors::sendDataToDAC(){
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

void I2cSensors::transferFunction(){
    double x = inputAccelerometer[0];
    double y = inputAccelerometer[1];
    // Serial.print("X: \t"); Serial.print(x);
    // Serial.print("Y: \t"); Serial.println(y); Serial.println();
    bool isActive = analogSensors.readPressure();

    uint16_t outputControlAux[2];

    if( x < -10) outputControlAux[0] = 2048 * (-x / 50 + 1);
    else if( x >= -10 && x <= 30) outputControlAux[0] = 2048;
    else outputControlAux[0] = 2048 * (-x / 90 + 1);

    if( y < -15) outputControlAux[1] = 2048 * (y / 50 + 1);
    else if( y >= -15 && y <= 15) outputControlAux[1] = 2048;
    else outputControlAux[1] = 2048 * (y / 85 + 1);

    if(outputControlAux[0] < 0) outputControlAux[0] = 0;
    else if(outputControlAux[0] > 4096 ) outputControlAux[0] = 4096;

    if(outputControlAux[1] < 0) outputControlAux[1] = 0;
    else if(outputControlAux[1] > 4096 ) outputControlAux[1] = 4096;

    if( isActive ) { //subir-bajar giro-sobre-si-mismo
        outputControl[0] = 2048;
        outputControl[1] = 2048;
        outputControl[2] = outputControlAux[1];
        outputControl[3] = outputControlAux[0];
    } else { // izq-dcha avace-retroceso
        outputControl[0] = outputControlAux[0];
        outputControl[1] = outputControlAux[1];
        outputControl[2] = 2048;
        outputControl[3] = 2048;
    }

}