

#include <Arduino_LSM6DSOX.h>

#define LED_PIN LED_BUILTIN
const float threshold = 2.0;

void setup() {

  Serial.begin(9600);
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
  
  
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void loop() {
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

      // Determine the axis with the maximum acceleration
      if (abs(x) > abs(y) && abs(x) > abs(z)) {
        // Red for X axis
        digitalWrite(LEDR, HIGH); // Red for X-axis
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, LOW);

      } else if (abs(y) > abs(x) && abs(y) > abs(z)) {
        // Green for Y axis
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, HIGH); // Green for Y-axis
        digitalWrite(LEDB, LOW);
      } else if (abs(z) > abs(x) && abs(z) > abs(y)){
        digitalWrite(LEDR, LOW); // Red + Green = Yellow for Z-axis
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, HIGH);
      }

    
    }
  }
}

