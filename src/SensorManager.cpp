#include "SensorManager.h"

// Constructor for SensorManager class
SensorManager::SensorManager() {
    // Define the I2C bus and the sensors
    i2c = new DevI2C(D14, D15);
    tempSensor = new HTS221Sensor(*i2c);
    pressureSensor = new LPS22HBSensor(*i2c);
}

// Initialize the sensors
void SensorManager::initSensors() {
    tempSensor->init(NULL);
    pressureSensor->init(NULL);
}

// Read the temperature and humidity sensor data
void SensorManager::readTempSensorData(float* currentTemperature, float* currentHumidity) {
    tempSensor->enable();
    tempSensor->getHumidity(currentHumidity);
    
    tempSensor->getTemperature(currentTemperature);
    *currentTemperature = *currentTemperature * 1.8 + 32;
    
    tempSensor->disable();
    tempSensor->reset();
}

// Read the pressure sensor data
void SensorManager::readPressureSensorData(float* currentPressure) {
    pressureSensor->getPressure(currentPressure);
}