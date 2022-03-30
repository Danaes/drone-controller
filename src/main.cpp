#include "headers/sensors.h" 

Sensors sensors;

void setup(){
    Serial.begin(9600);

    while (!Serial)
        delay(10);

    sensors.setup();
}

void loop(){
    sensors.loop();
    delay(200);
}