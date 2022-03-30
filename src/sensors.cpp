#include "headers/sensors.h"

I2cSensors i2cSensors;

void Sensors::setup(){
    i2cSensors.setup();
}

void Sensors::loop(){
    i2cSensors.loop();
}