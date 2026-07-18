# Coffre-fort Arduino — PIN Keypad

Secure lock system with 4-digit PIN, admin mode and 
visual/audio feedback using a 4x4 keypad and LCD display.

## Features
- 4-digit PIN authentication
- Wrong PIN → red LED + buzzer alert
- Correct PIN → green LED + 3s unlock
- Admin mode (#) to change PIN on the fly
- FSM (Finite State Machine) architecture

## Hardware
- Arduino Uno R3
- Keypad 4x4 membrane
- LCD 1602 + I2C module
- LED green + LED red
- Active buzzer
- Resistors 220Ω

## Wiring
| Component   | Arduino Pin |
|-------------|-------------|
| Keypad R1   | D2          |
| Keypad R2   | D3          |
| Keypad R3   | D4          |
| Keypad R4   | D5          |
| Keypad C1   | D6          |
| Keypad C2   | D7          |
| Keypad C3   | D8          |
| Keypad C4   | D9          |
| LCD SDA     | A4          |
| LCD SCL     | A5          |
| LED green   | D12          |
| LED red     | D13          |
| Buzzer      | D11          |

## Libraries
- Keypad — Mark Stanley
- LiquidCrystal I2C — Frank de Brabander

## Usage
| Key | Action |
|-----|--------|
| `*` | Start PIN entry / Cancel |
| `#` | Admin mode (change PIN) |
| `0-9` | Enter PIN digits |

## FSM States
LOCKED → PIN_ENTRY → GRANTED / DENIED → LOCKED
LOCKED → ADMIN → LOCKED
