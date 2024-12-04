#include <Arduino.h>
#include <RGB_LED.h>
#include <AZ3166WiFi.h>
#include <OledDisplay.h>


#include "WifiUtils.h"
#include "MqttUtils.h"
#include "SensorManager.h"

RGB_LED rgbLed;


SensorManager* sensorManager;

// Hold the previous temperature and humidity values that had been sent to the MQTT server.
float humidity = 0;
float temperature = 0;

void flashRGBLed(int red, int green, int blue) {
  rgbLed.setColor(red, green, blue);
  delay(1000);
  rgbLed.turnOff();
  delay(500);
  rgbLed.setColor(red, green, blue);
  delay(1000);
  rgbLed.turnOff();
}
void setup() {
  //Set the Serial output speed so we can see the output in the Serial Monitor
  Serial.begin(115200);
  
  //Initialize the LCD screen.
  Screen.init();
  
  //Initialize the sensors
  sensorManager = new SensorManager();
  sensorManager->initSensors();
}

void loop() {
  
  // Initialize or reconnect the Wifi
  initWifi();

  // Placeholders for current sensor data.
  float currentTemperature = 0;
  float currentHumidity = 0;
  float currentPressure = 0;

  // Read sensor data
  sensorManager->readTempSensorData(&currentTemperature, &currentHumidity);
  Serial.printf("Temperature: %.2f \n", currentTemperature);
  Serial.printf("Humidity: %.2f \n", currentHumidity);
  sensorManager->readPressureSensorData(&currentPressure);
  Serial.printf("Pressure: %.2f \n", currentPressure);

  // Print to OLED
  Screen.print(0, ("IP:" + String(WiFi.localIP().get_address())).c_str());
  Screen.print(1, ("Temp: " + String(currentTemperature)).c_str());
  Screen.print(2, ("Humidity: " + String(currentHumidity)).c_str());
  Screen.print(3, ("Pressure: " + String(currentPressure)).c_str());

  // If the current temperature or humidity is different from the previous one by at least 1,
  // then update the previous temperature and humidity and send the data to the MQTT server.
  if (abs(currentTemperature - temperature) >= 1 || abs(currentHumidity - humidity) >= 1) {
      
    int rc = sendMQTTMessage(&currentTemperature, &currentHumidity, &currentPressure);

    if (rc != 0) {
      Serial.printf("Message send failed %d \n", rc);
      flashRGBLed(255,0,0);
      return;
    }
    
    temperature = currentTemperature;
    humidity = currentHumidity;

    Serial.println("Message sent to MQTT server successfully");
    flashRGBLed(0,255,0);
  }
  else
  {
    Serial.println("No data change, no need to send to MQTT server");
    flashRGBLed(0,0,255);
  }

  // Sleep for the configured interval.  
  delay(SLEEP_INTERVAL);
}

