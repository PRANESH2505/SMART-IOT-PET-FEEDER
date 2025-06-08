#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

Servo myServo;

const int irSensorPin = 2;
const int servoPin = 6;
const int trigPin = 7;
const int echoPin = 8;

byte allowedUID[4] = {0xA2, 0x29, 0x78, 0x00};
byte deniedUID[4] = {0xBF, 0xFE, 0x26, 0x1F};

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool obstacleDetected = false;
bool lidOpened = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myServo.attach(servoPin);
  pinMode(irSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.write(0);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Pet Feeder");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}

void loop() {
  bool irState = digitalRead(irSensorPin);

  if (irState == HIGH && obstacleDetected) {
    // Pet moved away
    Serial.println("No Obstacle");
    showDepth();
    obstacleDetected = false;
    lidOpened = false;
  }
  else if (irState == HIGH && !obstacleDetected) {
    // No pet, normal state
   

  if (obstacleDetected && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (!lidOpened) {
      if (checkUID(rfid.uid.uidByte, allowedUID)) {
        Serial.println("Tom Detected, opening lid");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Tom Detected");
        myServo.write(90);
        lcd.setCursor(0, 1);
        lcd.print("Food Lid Open");
        lidOpened = true;
        delay(5000);
        myServo.write(0);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Lid Closed");
        lcd.setCursor(0, 1);
        lcd.print("Waiting...");
        Serial.println("Food Lid Closed");
      } else if (checkUID(rfid.uid.uidByte, deniedUID)) {
        Serial.println("Access Denied");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied");
        myServo.write(0);
      } else {
        Serial.println("Unknown Card");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Unknown Card");
        lcd.setCursor(0, 1);
        lcd.print("Access Denied");
        myServo.write(0);
      }
      rfid.PICC_HaltA();
    }
  }

  delay(200);
}

bool checkUID(byte *uid, byte *compareUID) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != compareUID[i]) return false;
  }
  return true;
}

void showDepth() {
  float distance = measureDistance();
  Serial.print("Depth = ");
  Serial.print(distance);
  Serial.println(" cm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("No Obstacle");
  lcd.setCursor(0, 1);
  lcd.print("Depth: ");
  lcd.print(distance);
  lcd.print(" cm");
}


  return duration * 0.034 / 2;
}

