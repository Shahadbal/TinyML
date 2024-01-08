// mqtt_simplesender.ino: WiFi Simple MQTT Sender
// Versione 1.0 del 16/11/2023
//
// Derived from: 
// https://gist.github.com/Rajavasanthan/2e529bbb85e1682173267234446295cf
// 
// See this tutorial:
// https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-mqtt-device-to-device
// 
// This example connects to a MQTT broker and publishes a message to
// a topic once a second.
// Board: Nicla Vision
// 
// This example code is in the public domain.
// 
// Libraries to be installed:
// 1) WiFi: https://github.com/arduino-libraries/WiFi
// 2) ArduinoMqttClient: https://github.com/arduino-libraries/ArduinoMqttClient
// 
// For the Nicla it is necessary to upgrade the WiFi firmware and the bootloader:
// See: https://forum.arduino.cc/t/arduino-nicla-vision-is-not-connecting-to-wifi/1032159
// And: (https://support.arduino.cc/hc/en-us/articles/8468641365276-Update-the-bootloader-on-Nicla-Vision)

//
// This code send data via MQTT: every second, the terminal will show messages such as:
// hello 1434
// Sending message to topic: task
// hello 1435
// Sending message to topic: task
// hello 1436
// ...
// To run the MQTT subscriber:
// Open the serial monitor and launch the command mosquitto_sub:
// mosquitto_sub -h test.mosquitto.org -t task
//
// Examples of messages received by the subscriber:
// hello 1434
// hello 1435
// hello 1436
// ...
//
//

#include <ArduinoMqttClient.h>

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#endif
#include <Arduino_LSM6DSOX.h>

#define LED_PIN LED_BUILTIN
const float threshold = 2.0;
// #include "arduino_secrets.h"

// --------------------------------
// dati connessione WIFI
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
// char ssid[] = SECRET_SSID;    	// your network SSID (name)
// char pass[] = SECRET_PASS;    	// your network password (use for WPA, or use as key for WEP)
char ssid[] = "Tiny-ML";       	// your network SSID (name)
char pass[] = "12345677";       	// your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.
// --------------------------------

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// --------------------------------
// DATI DEL BROKER MQTT
// const char broker[] = "192.168.1.3";
// int        port     = 5672;
// 
const char broker[] = "broker.hasanabs.com";
int        port     = 1883;
// --------------------------------
//
const char topic[]  = "kaust";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
	//Initialize serial and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// attempt to connect to WiFi network:
	Serial.print("Attempting to connect to WPA SSID: ");
	Serial.println(ssid);
	Serial.println(pass);
	while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
		// failed, retry
		Serial.print(".");
		delay(5000);
	}

	Serial.println("You're connected to the network");
	Serial.println();

	// You can provide a unique client ID, if not set the library uses Arduino-millis()
	// Each client must have a unique client ID
	// mqttClient.setId("clientId");

	// You can provide a username and password for authentication
	// mqttClient.setUsernamePassword("username", "password");

	Serial.print("Attempting to connect to the MQTT broker: ");
	Serial.println(broker);

	if (!mqttClient.connect(broker, port)) {
		Serial.print("MQTT connection failed! Error code = ");
		Serial.println(mqttClient.connectError());

		while (1);
	}

	Serial.println("You're connected to the MQTT broker!");
	Serial.println();

  while (!Serial);

  pinMode(LED_PIN, OUTPUT);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("X\tY\tZ");
}

void loop() {
	// call poll() regularly to allow the library to send MQTT keep alives which
	// avoids being disconnected by the broker
	mqttClient.poll();

	// to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
	// see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
	unsigned long currentMillis = millis();


  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);

    // Check for acceleration greater than the threshold
    if (abs(x) > threshold || abs(y) > threshold || abs(z) > threshold) {
      Serial.println("Acceleration exceeds threshold!");
      if (currentMillis - previousMillis >= interval) {
		// save the last time a message was sent
        previousMillis = currentMillis;

        Serial.print("Sending message to topic: ");
        Serial.println(topic);
        Serial.print("hello Shahad");
        Serial.println(count);

        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic);
        mqttClient.print("hello Shahad");
        mqttClient.print(count);
        mqttClient.endMessage();

        Serial.println();

        count++;
      }



   } 
}
}