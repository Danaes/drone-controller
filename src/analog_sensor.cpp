#include "headers/analog_sensors.h"

const int pressureSensor = A0;


bool AnalogSensors::readPressure(){
    int inputSensor = analogRead(pressureSensor);
    Serial.print("Presion: "); Serial.print(inputSensor); Serial.println();

    return (inputSensor > 50) ? true : false;
}