# Weather Station — Arduino Uno

Real-time temperature, humidity and time display 
using DHT11, LM35 and DS1302 on a 16x2 LCD.

## Hardware
- Arduino Uno R3
- DHT11 (temperature + humidity)
- LM35 (analog temperature)
- DS1302 RTC module (real-time clock)
- LCD 1602 + I2C module

## Wiring
| Component | Pin        |
|-----------|------------|
| DHT11 DATA | D2 + 10kΩ pull-up |
| LM35 OUT  | A1         |
| DS1302 CLK | D6        |
| DS1302 DAT | D7        |
| DS1302 RST | D8        |
| LCD SDA   | A4         |
| LCD SCL   | A5         |

## Libraries
- DHT sensor library — Adafruit
- LiquidCrystal I2C — Frank de Brabander
- Rtc by Makuna

## Notes
- LM35 readings averaged over 10 samples for accuracy
- DS1302 keeps time even when Arduino is powered off
