#include "VL53L1X.h"
VL53L1X proximity;

int reading = 0;
int maxBlinkTime = 2000; // Maximum time interval for LED blink
int minBlinkTime = 200;  // Minimum time interval for LED blink

void setup() {
  Serial.begin(115200);
  Wire1.begin();
  Wire1.setClock(400000); // use 400 kHz I2C
  proximity.setBus(&Wire1);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  if (!proximity.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }

  proximity.setDistanceMode(VL53L1X::Short);
  proximity.setMeasurementTimingBudget(10000);
  proximity.startContinuous(10);
}

void loop() {
  reading = proximity.read();
  Serial.println(reading);

  // Map the reading to the RGB color scale (yellow to red)
  int redValue = map(reading, 0, 1000, 255, 0);
  int greenValue = map(reading, 0, 1000, 255, 255);
  int blueValue = map(reading, 0, 1000, 0, 0);

  analogWrite(LEDR, redValue);
  analogWrite(LEDG, greenValue);
  analogWrite(LEDB, blueValue);

  // Adjust LED blink rate based on proximity reading
  int blinkTime = map(reading, 0, 1000, maxBlinkTime, minBlinkTime);

  // Implement LED blinking
  static unsigned long timeStart = millis();
  if (millis() - timeStart >= blinkTime) {
    timeStart = millis();
    // Toggle LED state
    digitalWrite(LEDR, !digitalRead(LEDR));
    digitalWrite(LEDG, !digitalRead(LEDG));
    digitalWrite(LEDB, !digitalRead(LEDB));
  }
}
