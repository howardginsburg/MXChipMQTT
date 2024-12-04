#include <Arduino.h>
#include "AzureIotHub.h" // Not used, but fixes dependency issues with other AZ3166 libraries.
//#include <RGB_LED.h>
#include <AZ3166WiFi.h>
#include "HTS221Sensor.h"
#include <LPS22HBSensor.h>
#include <OledDisplay.h>
#include "MQTTClient.h"
#include "MQTTNetwork.h"

//RGB_LED rgbLed;

DevI2C *i2c;
HTS221Sensor *tempSensor;
LPS22HBSensor *pressureSensor;
float humidity = 0;
float temperature = 0;

void InitWifi()
{
  // If the Wifi Status is not connected or the ip address is 0.0.0.0, then connect to the Wifi.
  while ((WiFi.status() != WL_CONNECTED) || (String(WiFi.localIP().get_address()) == "0.0.0.0"))
  {
    Serial.printf("Connecting to WIFI SSID %s \n", WIFI_SSID);
    Screen.clean();
    Screen.print(1, "Connecting WIFI...");
    Screen.print(2, ("SSID:" + String(WIFI_SSID)).c_str());
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

void readTempSensorData(float* currentTemperature, float* currentHumidity)
{
  // enable
  tempSensor -> enable();
  
  // get humidity
  tempSensor -> getHumidity(currentHumidity);
  Serial.printf("Humidity: %.2f \n", *currentHumidity);
  
  // get temperature and convert to Fahrenheit
  tempSensor -> getTemperature(currentTemperature);
  *currentTemperature = *currentTemperature * 1.8 + 32;
  Serial.printf("Temperature: %.2f \n", *currentTemperature);

  // disable the sensor
  tempSensor -> disable();
  // reset
  tempSensor -> reset();
}

void readPressureSensorData(float* currentPressure)
{
  // get pressure
  pressureSensor -> getPressure(currentPressure);
  Serial.printf("Pressure: %.2f \n", *currentPressure);
}

int sendMQTTMessage(float* currentTemperature, float* currentHumidity, float* currentPressure)
{
  MQTTNetwork mqttNetwork;
  MQTT::Client<MQTTNetwork, Countdown> client = MQTT::Client<MQTTNetwork, Countdown>(mqttNetwork);
  
  Serial.printf("Connecting to MQTT server %s:%d \n", MQTT_BROKER, MQTT_PORT);
  int rc = mqttNetwork.connect(MQTT_BROKER, MQTT_PORT);
  if (rc != 0) {
    Serial.printf("Connected to MQTT server failed %d \n", rc);
    return rc;
  }

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  data.clientID.cstring = MQTT_CLIENT_ID;
  data.username.cstring = MQTT_USERNAME;
  data.password.cstring = MQTT_PASSWORD;

  if ((rc = client.connect(data)) != 0) {
    Serial.println("MQTT client connect to server failed");
    return rc;
  }
  
  Serial.printf("Connecting to MQTT topic %s \n", MQTT_TOPIC);
  MQTT::Message message;
  // QoS 0
  char buf[200];
  sprintf(buf, "{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f}", *currentTemperature, *currentHumidity, *currentPressure);
  
  message.qos = MQTT::QOS0;
  message.retained = true;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf)+1;
  if (client.publish(MQTT_TOPIC, message) != 0) {
    Serial.printf("Message send failed %d \n", rc);
    return rc;
  }
  
  if ((rc = client.disconnect()) != 0) {
    Serial.printf("MQTT Client Disconnect failed %d \n", rc);
    return rc;
  }
  
  if ((rc = mqttNetwork.disconnect()) != 0) {
    Serial.printf("MQTT Network Disconnect failed %d \n", rc);
    return rc;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Screen.init();
  
  i2c = new DevI2C(D14, D15);
  tempSensor = new HTS221Sensor(*i2c);
  // init the sensor
  tempSensor -> init(NULL);

  pressureSensor = new LPS22HBSensor(*i2c);
  // init the sensor
  pressureSensor -> init(NULL);

}

void loop() {
    Screen.clean();

    InitWifi();

    float currentTemperature = 0;
    float currentHumidity = 0;
    float currentPressure = 0;

    readTempSensorData(&currentTemperature, &currentHumidity);
    readPressureSensorData(&currentPressure);

    // Print to OLED
    //Screen.print(0, ("SSID:" + String(WiFi.SSID())).c_str());
    Screen.print(0, ("IP:" + String(WiFi.localIP().get_address())).c_str());
    Screen.print(1, ("Temperature: " + String(currentTemperature)).c_str());
    Screen.print(2, ("Humidity: " + String(currentTemperature)).c_str());
    Screen.print(3, ("Pressure: " + String(currentPressure)).c_str());

    // If the current temperature or humitidity is different from the previous one by at least 1,
    // then update the previous temperature and humidity and send the data to the MQTT server.
    if (abs(currentTemperature - temperature) >= 1 || abs(currentHumidity - humidity) >= 1) {
        
      int rc = sendMQTTMessage(&currentTemperature, &currentHumidity, &currentPressure);

      if (rc != 0) {
        Serial.printf("Message send failed %d \n", rc);
        return;
      }
      
      temperature = currentTemperature;
      humidity = currentHumidity;

      Serial.println("Message sent to MQTT server successfully");
    }
    else
    {
      Serial.println("No data change, no need to send to MQTT server");
    }
      
    delay(SLEEP_INTERVAL);
}

// void loop() {

//   digitalWrite(LED_USER, HIGH);
//   digitalWrite(LED_WIFI, HIGH);
//   digitalWrite(LED_AZURE, HIGH);
//   rgbLed.setColor(0,0,255);
//   delay(1000);
//   digitalWrite(LED_USER, LOW);
//   digitalWrite(LED_WIFI, LOW);
//   digitalWrite(LED_AZURE, LOW);
//   //turn off the rgb led
//   rgbLed.turnOff();
//   delay(1000);
// }

