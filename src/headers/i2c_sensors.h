#ifndef _i2c_sensors_h_
#define _i2c_sensors_h_

#include "Adafruit_MMA8451.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_MCP4728.h>
#include "analog_sensors.h"

extern AnalogSensors analogSensors;

class I2cSensors {
private:
    void setUpAccelerometer();
    void setUpDAC();
    void setUpEngines();
    void readAccelerometer();
    void sendDataToDAC();
    void transferFunction();
public:
    void setup();
    void loop();
};

#endif