#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <AltSoftSerial.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// Using a soft serial port
SoftwareSerial softSerial(11, 10);

MPU6050 mpu;
DFRobotDFPlayerMini myDFPlayer;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;

void setup() {

  Serial.begin(115200);
  softSerial.begin(9600);

  /////////////////////////////////////////////////////////
  // Display Initialization
  /////////////////////////////////////////////////////////
  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif // RST_PIN >= 0

  oled.setFont(TimesNewRoman16_bold);
  oled.clear();
  // oled.print("Hello world!");
  delay(500);

  /////////////////////////////////////////////////////////
  // DFMiniPlayer Initialization
  /////////////////////////////////////////////////////////
  if (!myDFPlayer.begin(softSerial, true, true)) {
    Serial.println("DFPlayer initialization failed");
    while (true);
  }
  Serial.println("DFPlayer initialized successfully");
  

  /////////////////////////////////////////////////////////
  // Accelerometer Initialization
  /////////////////////////////////////////////////////////
  mpu.initialize();  // Initialize MPU6050
  

  /////////////////////////////////////////////////////////
  // Pin Initializations
  /////////////////////////////////////////////////////////
  pinMode(8, INPUT);      // Button input
  digitalWrite(8, HIGH);  // Button initialization

  pinMode(A0, INPUT);  // x axis
  pinMode(A1, INPUT);  // y axis

  delay(500);             // Allow time for initialization
  myDFPlayer.volume(13);  //Set volume value. From 0 to 30
  //myDFPlayer.play(1);
  delay(1000);     
}

// Input 1: Craft a block 
// outputs: 
//inputs: 
bool craft() {
  myDFPlayer.play(4);
  delay(1000);
  // HIGH = idle    LOW = pressed
  bool flag = false;
  
  while (!flag) {
    if(digitalRead(8)==LOW) {
      myDFPlayer.play(1);
      Serial.println("Craft!");
      flag = true;
      delay(500);
    }
  }
  return flag;
}

// Input 2: Dodge an attack/enemy
// outputs: 
// inputs:
bool dodge() {
  myDFPlayer.play(5);
  delay(1000);
  bool flag = false;
  bool dodged = false;

  while (!flag) {
    int xVal = analogRead(A0);
    int yVal = analogRead(A1);

    if ((xVal >= 600 && xVal <= 1000) && (yVal >= 0 && yVal <= 500) || 
        (xVal >= 500 && yVal >= 1000) || 
        (xVal >= 0 && xVal <= 100) && (yVal >= 500) || 
        (xVal >= 1000 && yVal >= 500)) {
      dodged = true;
      flag = true;
      Serial.println("Dodged!");
      delay(500);
    }
  }
  return dodged;
}

// Input 3: Attack
// outputs:
// inputs:
bool attack() {
  myDFPlayer.play(6);
  delay(1000);
  bool flag = false;
  bool swing = false;

  while(!flag) {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    // Read accelerometer and gyroscope data
    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);

    if (ay < 0) {
      Serial.println("Swung");
      myDFPlayer.play(8);
      flag = true;
      swing = true;
      delay(500);
    }
  }
  
  return swing;
}

bool startGame() {
  bool pressed = false;
  while (pressed == false) {
    if (digitalRead(8) == LOW) {
      pressed = true;
      Serial.println("Game Begins!");
      myDFPlayer.play(3); // Game begins
      delay(1000);
    }
  }
  return pressed;
}

void loop() {
  int i=200;  //timer in milliseconds 
  int sub = 20;  //subtract from timer if player gives correct input
  int score = 0;  //score count
  int y;
  bool success = false;

  startGame();

  while (i >= 0) {
    bool success = false;
    int action = random(1,4);
    Serial.print("Action chosen: ");
    Serial.println(action); // 1-3
    delay(1000);
    
    if (action == 1) { 
      success = craft();
      Serial.print("Succeed: ");
      Serial.println(success);
      delay(200);
    }
    else if (action == 2) {
      success = dodge();
      Serial.print("Succeed: ");
      Serial.println(success);
      delay(200);
    }
    else if (action == 3) {
      success = attack();
      Serial.print("Succeed: ");
      Serial.println(success);
      delay(200);
    }

    
    if (success == true) {
      score++;
      i -= sub;
      Serial.print("Score: ");
      oled.clear();
      Serial.println(score);
      oled.print(score);
      delay(500);
    }
    else {
      myDFPlayer.play(7); // Game over
      Serial.println("Game Over");
      return 0;
    }
  }

  if (i <= 0) {
    Serial.println("You Win!");
    Serial.print("Score: ");
    Serial.println(score);
  }

  return 0;
}
