#include "VL53L1X.h"
VL53L1X proximity;

const int redLedPin = 13; // Change this to the actual pin where your red LED is connected
const int distanceThreshold = 30; // Set your desired threshold distance in centimeters

void setup() {
  Serial.begin(115200);
  Wire1.begin();
  Wire1.setClock(400000); // use 400 kHz I2C
  proximity.setBus(&Wire1);

  pinMode(redLedPin, OUTPUT);

  if (!proximity.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }

  proximity.setDistanceMode(VL53L1X::Short);
  proximity.setMeasurementTimingBudget(10000);
  proximity.startContinuous(10);
}

void loop() {
  int distance = proximity.read();
  Serial.println(distance);

  if (distance < distanceThreshold) {
    // Person is too close, blink the red LED
    blinkRedLED();
  } else {
    // Person is at a safe distance, turn off the red LED
    digitalWrite(redLedPin, LOW);
  }
}

void blinkRedLED() {
  static unsigned long previousMillis = 0;
  const int blinkInterval = 500; // Blink interval in milliseconds

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= blinkInterval) {
    // Toggle the state of the red LED
    digitalWrite(redLedPin, !digitalRead(redLedPin));
    previousMillis = currentMillis;
  }
}

