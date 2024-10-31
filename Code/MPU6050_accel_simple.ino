#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

// Using a soft serial port
SoftwareSerial softSerial(10, 11);

MPU6050 mpu;
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Wire.begin();  // Initialize I²C

  softSerial.begin(9600);

  Serial.begin(115200);

  if (!myDFPlayer.begin(softSerial, true, true)) {
    Serial.println("DFPlayer initialization failed");
    while (true)
      ;
  }
  Serial.println("DFPlayer initialized successfully");

  mpu.initialize();  // Initialize MPU6050

  pinMode(8, INPUT);      // Button input
  digitalWrite(8, HIGH);  // Button initialization

  pinMode(A0, INPUT);  // x axis
  pinMode(A1, INPUT);  // y axis

  delay(500);             // Allow time for initialization
  myDFPlayer.volume(100);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);     //Play the first mp3
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int xVal = analogRead(A0);
  int yVal = analogRead(A1);

  // Read accelerometer and gyroscope data
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  // Print data to Serial Monitor
  // Serial.print("aX: ");
  // Serial.println(ax);
  // Serial.print("aY: ");
  // Serial.println(ay);
  // Serial.print("aZ: ");
  // Serial.println(az);
  // Serial.print(" ");
  //Serial.print(" | gX: "); Serial.print(gx);
  //Serial.print(" | gY: "); Serial.print(gy);
  //Serial.print(" | gZ: "); Serial.println(gz);
  //delay(5000);

  if (ay < 0) {
    Serial.print("Swung");
    delay(500);
  }

  if (digitalRead(8) == LOW) {
    Serial.print("Button Pressed");
    delay(500);
  }

  // Serial.print("X: ");
  // Serial.println(xVal);
  // Serial.print("Y: ");
  // Serial.print(yVal);

  // Up
  if ((xVal >= 600 && xVal <= 1000) && (yVal >= 0 && yVal <= 500)) {
    Serial.print("Up");
    delay(500);
  } else if ((xVal >= 500 && yVal >= 1000)) {
    Serial.print("Down");
    delay(500);
  } else if ((xVal >= 1000 && yVal >= 500)) {
    Serial.print("Right");
    delay(500);
  } else if ((xVal >= 0 && xVal <= 100) && (yVal >= 500)) {
    Serial.print("Left");
    delay(500);
  }
}
