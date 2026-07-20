# MPU6050 Digital Level — Complementary Filter

Real-time inclinometer using MPU6050 IMU with a hand-coded 
complementary filter for accurate 
roll/pitch estimation.

## How it works
The complementary filter combines two imperfect sensors:
- Accelerometer: accurate long-term but noisy
- Gyroscope: accurate short-term but drifts over time

angle = α × (angle + gyro × dt) + (1-α) × angle_accel
(α = 0.98)

Auto-calibration on startup averages 100 samples to remove
accelerometer and gyroscope offsets.

## Hardware
- Arduino Uno R3
- MPU6050 IMU (I2C)
- LCD 1602 + I2C module
- RGB LED (red/green/blue)
- Passive buzzer

## Wiring
| Component   | Arduino Pin |
|-------------|-------------|
| MPU6050 SDA | A4          |
| MPU6050 SCL | A5          |
| LCD SDA     | A4          |
| LCD SCL     | A5          |
| LED red     | D13          |
| LED green   | D12         |
| LED blue    | D11         |
| Buzzer      | D6          |

## Thresholds
| Angle     | LED   | Status    |
|-----------|-------|-----------|
| < 25°      | Green | LEVEL OK  |
| 25° - 35°  | Blue  | ATTENTION |
| 35° - 40° | Red   | DANGER    |
| > 40°     | Red   | ALARM     |

## Libraries
- MPU6050 — Jeff Rowberg (i2cdevlib)
- I2Cdev — Jeff Rowberg
- LiquidCrystal I2C — Frank de Brabander
