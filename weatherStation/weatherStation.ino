#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

// Pin def
#define DHT_PIN     2       
#define DHT_TYPE    DHT11
#define LM35_PIN    A1      

// objects creation

// DS1302 : CLK=6, DAT=7, RST=8
ThreeWire myWire(7, 6, 8);
RtcDS1302<ThreeWire> Rtc(myWire);

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT
DHT dht(DHT_PIN, DHT_TYPE);

// global variables
float tempDHT, humidity, tempLM35;
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

// just to create the degree symbol
byte degreeChar[8] = {
  0b00110, 0b01001, 0b01001, 0b00110,
  0b00000, 0b00000, 0b00000, 0b00000
};


void setup() {
  Serial.begin(9600);

  // LCD Initialisation 
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeChar);
  lcd.setCursor(0, 0);
  lcd.print("Station Meteo");
  lcd.setCursor(0, 1);
  lcd.print("Demarrage...");
  delay(2000);
  lcd.clear();

  // DHT11 Initialisation
  dht.begin();

  // RTC DS1302 Initialisation 
  Rtc.Begin();

  // This need to be done only one time to configure time

  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //Rtc.SetDateTime(compiled);
  //Rtc.SetIsRunning(true);

  Serial.println("Setup done");
}


void readSensors() {
  // DHT11
  tempDHT  = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(tempDHT) || isnan(humidity)) {
    Serial.println("Erreur lecture DHT11");
    tempDHT  = -99;
    humidity = -99;
  }

  // LM35
  // Analog value so we need to do conversion : voltage = (raw_value / 1023) * 5000 mV
  // LM35 : 10 mV / °C → température = tension / 10
  int raw = analogRead(LM35_PIN);
  tempLM35 = (raw * 5000.0 / 1023.0) / 10.0;
}


void printLCD(RtcDateTime now) {
  lcd.clear();

  // row 0 : time + DHT11 temperature
  lcd.setCursor(0, 0);

  // Time (HH:MM:SS format)
  if (now.Hour()   < 10) lcd.print("0");
  lcd.print(now.Hour());   lcd.print(":");
  if (now.Minute() < 10) lcd.print("0");
  lcd.print(now.Minute()); lcd.print(" ");

  // DHT11 temperature
  lcd.print(tempDHT, 1);
  lcd.write(0);
  lcd.print("C");

  // row 1 : LM35 humidity + temperature 
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("% L:");
  lcd.print(tempLM35, 1);
  lcd.write(0);
  lcd.print("C");
}


void printSerial(RtcDateTime now) {
  Serial.print("Hour : ");
  Serial.print(now.Hour());   Serial.print(":");
  if (now.Minute() < 10) Serial.print("0");
  Serial.print(now.Minute()); Serial.print(":");
  if (now.Second() < 10) Serial.print("0");
  Serial.println(now.Second());

  Serial.print("DHT11 - Temp : ");
  Serial.print(tempDHT);
  Serial.print(" C  |  Humidite : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("LM35  - Temp : ");
  Serial.print(tempLM35);
  Serial.println(" C");
  Serial.println("---");
}


void loop() {
  unsigned long now_ms = millis();

  if (now_ms - lastRead >= READ_INTERVAL) {
    lastRead = now_ms;

    readSensors();

    RtcDateTime now = Rtc.GetDateTime();
    printLCD(now);
    printSerial(now);
  }
}