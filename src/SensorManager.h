#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"

class SensorManager {
public:
    SensorManager();
    void initSensors();
    void readTempSensorData(float* currentTemperature, float* currentHumidity);
    void readPressureSensorData(float* currentPressure);

private:
    DevI2C *i2c;
    HTS221Sensor* tempSensor;
    LPS22HBSensor* pressureSensor;
};