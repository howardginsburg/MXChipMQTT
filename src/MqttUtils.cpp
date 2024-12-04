#include "AzureIotHub.h" // Not used, but fixes dependency issues with other AZ3166 libraries.
#include "MQTTClient.h"
#include "MQTTNetwork.h"

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
  message.payloadlen = strlen(buf);//+1;
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