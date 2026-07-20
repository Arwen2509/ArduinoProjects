#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>

// PIN
#define LED_RED 13
#define LED_GREEN 12
#define LED_BLUE 11
#define BUZZER    6

// Objects
MPU6050 mpu(0x68);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Filter
const float ALPHA = 0.98;
float angleX = 0, angleY = 0;
unsigned long lastTime = 0;

// Thresholds
const float SEUIL_GREEN  = 25.0;
const float SEUIL_JAUNE = 35.0;
const float SEUIL_RED = 40.0;

float offsetX = 0, offsetY = 0;
float offsetGyroX = 0, offsetGyroY = 0;


void calibrate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration...");
  lcd.setCursor(0, 1);
  lcd.print("Don't move !");

  float sumAccX = 0, sumAccY = 0;
  float sumGyroX = 0, sumGyroY = 0;  // ← ajout

  for (int i = 0; i < 100; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float accX = ax / 16384.0;
    float accY = ay / 16384.0;
    float accZ = az / 16384.0;

    sumAccX += atan2(accY, accZ) * 180.0 / PI;
    sumAccY += atan2(accX, accZ) * 180.0 / PI;
    sumGyroX += gx / 131.0;
    sumGyroY += gy / 131.0;
    delay(10);
  }

  offsetX     = sumAccX  / 100.0;
  offsetY     = sumAccY  / 100.0;
  offsetGyroX = sumGyroX / 100.0;  // ← ajout
  offsetGyroY = sumGyroY / 100.0;  // ← ajout

  angleX = offsetX;
  angleY = offsetY;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrated !");
  delay(1000);
  lcd.clear();
}

void updateAngles() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;

  float gyroX = (gx / 131.0) - offsetGyroX;
  float gyroY = (gy / 131.0) - offsetGyroY;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  float angleAccX = atan2(accY, accZ) * 180.0 / PI - offsetX;
  float angleAccY = atan2(accX, accZ) * 180.0 / PI - offsetY;

  angleX = ALPHA * (angleX + gyroX * dt) + (1 - ALPHA) * angleAccX;
  angleY = ALPHA * (angleY + gyroY * dt) + (1 - ALPHA) * angleAccY;
}

void setLED(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  digitalWrite(LED_BLUE, blue);
}

void updateFeedback(float maxAngle) {
  if (maxAngle <= SEUIL_GREEN) {
    setLED(false, true, false);
    noTone(BUZZER);
  } else if (maxAngle <= SEUIL_JAUNE) {
    setLED(false, false, true);
    noTone(BUZZER);
  } else if (maxAngle <= SEUIL_RED) {
    setLED(true, false, false);
    tone(BUZZER, 1000, 100);
  } else {
    setLED(true, false, false);
    tone(BUZZER, 2000, 50);
  }
}

void printLCD() {
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(angleX, 1);
  lcd.print((char)223);
  lcd.print(" Y:");
  lcd.print(angleY, 1);
  lcd.print((char)223);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  float maxAngle = max(abs(angleX), abs(angleY));
  if (maxAngle <= SEUIL_GREEN) {
    lcd.print("  LEVEL OK     ");
  } else if (maxAngle <= SEUIL_JAUNE) {
    lcd.print("  WARNING     ");
  } else {
    lcd.print("  DANGER !      ");
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(200);
  mpu.initialize();
  calibrate();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Leveling scale");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  delay(2000);
  lcd.clear();

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  lastTime = millis();
}

void loop() {
  updateAngles();

  float maxAngle = max(abs(angleX), abs(angleY));
  updateFeedback(maxAngle);
  printLCD();

  Serial.print("X: "); Serial.print(angleX, 1);
  Serial.print("°  Y: "); Serial.print(angleY, 1);
  Serial.println("°");

  delay(50);
}
