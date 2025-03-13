# MXChip Sending Sensor Data via MQTT

This project represents a simple example of sending sensor data from an [MXChip (AZ3166)](https://microsoft.github.io/azure-iot-developer-kit/) device to a standard MQTT broker rather than Azure IoT Hub.  The MXChip device is a development board that includes a variety of sensors and can be programmed using [PlatformIO](https://platformio.org/). The device is connected to the network via Wi-Fi and sends sensor data to a MQTT broker.  Note, this sample does not support TLS, so it is not secure.  

Further down, I include instructions for having the data get picked up by [Home Assistant](https://www.home-assistant.io/) via a Mosquitto MQTT broker. Home Assistant is a popular open-source home automation platform that can be used to control and automate smart home devices. Home Assistant can be configured to receive sensor data from the MQTT broker and display it in the user interface.

## Prerequisites

1. [MXChip (AZ3166)](https://microsoft.github.io/azure-iot-developer-kit/)
1. [PlatformIO](https://platformio.org/) installed in [Visual Studio Code](https://code.visualstudio.com/)

## MXChip Device Setup

1. Clone this repository to your local machine.
1. Open Visual Studio Code to the project directory.
1. Rename [./platformio.sample.ini](./platformio.sample.ini) to `platformio.ini`.
1. Update the `platformio.ini` file with your Wi-Fi network credentials and MQTT broker information.  If you have multiple boards, make sure you give each one a unique client id and mqtt topic to publish to.
1. Build and upload the project to the MXChip device.

### Understanding the flashing lights!

1. When the device connects to Wifi, the small green LED will turn on.
1. As the device reads data, the large LED will flash to indicate:
   - Green: Successfully sent data to the MQTT broker
   - Blue: Data has not changed enough to send to the MQTT broker
   - Red: Failed to send data to the MQTT broker.

## Home Assistant Setup

1. [Home Assistant](https://www.home-assistant.io/) installed on a Raspberry Pi or other device.
1. An MQTT broker such as [Mosquitto](https://mosquitto.org/) installed and [Home Assistant](https://www.home-assistant.io/integrations/mqtt#setting-up-a-broker) connected to it.
1. Add the following configuration to your `configuration.yaml` file. This will create two sensors in Home Assistant that will receive the sensor data from the MXChip device.  Repeast as needed for multiple boards.

```yaml
mqtt:
  sensor:
    - name: "Basement Temperature"
      state_topic: "mxchip/basement/sensors"
      suggested_display_precision: 1
      unit_of_measurement: "°F"
      value_template: "{{ value_json.temperature }}"
    - name: "Basement Humidity"
      state_topic: "mxchip/basement/sensors"
      unit_of_measurement: "%"
      value_template: "{{ value_json.humidity }}"
```