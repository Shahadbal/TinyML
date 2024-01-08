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

char ssid[] = "Tiny-ML";       // Replace with your WiFi SSID
char pass[] = "12345677";   // Replace with your WiFi password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.hasanabs.com";
int port = 1883;

const char topicRaw[] = "raw";
const char topicAlert[] = "kaust";

const long interval = 1000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  Serial.println(pass);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

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
  mqttClient.poll();

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

      // Send raw data to the raw data topic
      mqttClient.beginMessage(topicRaw);
      mqttClient.print("Raw Data: X=");
      mqttClient.print(x);
      mqttClient.print(", Y=");
      mqttClient.print(y);
      mqttClient.print(", Z=");
      mqttClient.print(z);
      mqttClient.endMessage();

      // Send an alert message to the alert topic
      mqttClient.beginMessage(topicAlert);
      mqttClient.print("Alert: Acceleration exceeds threshold!");
      mqttClient.endMessage();

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        Serial.print("Sending message to topics: ");
        Serial.print(topicRaw);
        Serial.print(" and ");
        Serial.println(topicAlert);

        count++;
      }
    }
  }
}

