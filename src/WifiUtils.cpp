#include <AZ3166WiFi.h>

//Connects to the wifi network.
void initWifi()
{
  // If the Wifi Status is not connected or the ip address is 0.0.0.0, then connect to the Wifi.
  while ((WiFi.status() != WL_CONNECTED) || (String(WiFi.localIP().get_address()) == "0.0.0.0"))
  {
    Serial.printf("Connecting to WIFI SSID %s \n", WIFI_SSID);
    
    Screen.print(1, "Connecting WIFI...");
    Screen.print(2, ("SSID:" + String(WIFI_SSID)).c_str());
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}