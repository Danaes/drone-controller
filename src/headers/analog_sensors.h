#ifndef _analogSensors_h_
#define _analogSensors_h_

#include <Arduino.h>
#include <Wire.h>

class AnalogSensors {
public:
    bool readPressure();
};

#endif