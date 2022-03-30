#ifndef _sensors_h_
#define _sensors_h_

#include "headers/i2c_sensors.h"

extern I2cSensors i2cSensors;

// Patron de diseño estructural facade

class Sensors {
public:
    void setup();
    void loop();
};

#endif
