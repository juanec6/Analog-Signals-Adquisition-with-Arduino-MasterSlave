# Arduino Master-Slave Monitoring System

Monitoring system based on Arduino using I2C master-slave architecture. The slave (Nano) handles all hardware logic while the master (Uno) manages the display and menu rendering.

---

## Architecture

```
┌─────────────────────────────┐        I2C (5 bytes)       ┌──────────────────────────────┐
│       Arduino Nano          │ ─────────────────────────► │       Arduino Uno            │
│         (Slave)             │                             │         (Master)             │
│                             │                             │                              │
│  - Joystick navigation      │                             │  - LCD 16x2 display          │
│  - Enter / Back buttons     │                             │  - RTC DS3231                │
│  - Servo motor              │                             │  - Menu rendering            │
│  - Ultrasonic HC-SR04       │                             │  - Case-based display logic  │
│  - Menu state management    │                             │                              │
└─────────────────────────────┘                             └──────────────────────────────┘
```

---

## I2C Protocol

The Nano sends a fixed-byte packet to the Uno on every request:

| Byte | Content         | Type     | Range   |
|------|-----------------|----------|---------|
| 0    | `menuLevel`     | uint8_t  | 0–6     |
| 1    | `currentOption` | int8_t   | 0–4     |
| 2    | `servoPos`      | uint8_t  | 0–180   |
| 3    | `distance HIGH` | uint8_t  | —       |
| 4    | `distance LOW`  | uint8_t  | —       |

Distance is reconstructed on the master side as:
```cpp
distance = (distH << 8) | distL;
```

---

## Menu Structure

```
[case 0] Home — Time and date (RTC)
    ↓ ENTER
[case 1] Menu — Joystick navigation
    ↓ ENTER
[case 2] Radar       — Auto servo sweep + ultrasonic distance
[case 3] Logs        — Detection history
[case 4] Manual Op   — Joystick-controlled servo position
[case 5] Time Settings
[case 6] Alarm
    ↑ BACK (returns one level up)
```

---

## Hardware

### Arduino Nano (Slave)
| Component         | Pin  |
|-------------------|------|
| Enter button      | D3   |
| Back button       | D6   |
| Red LED           | D5   |
| Green LED         | D4   |
| Servo motor       | D2   |
| Ultrasonic TRIG   | D8   |
| Ultrasonic ECHO   | D7   |
| Joystick X        | A0   |
| Joystick Y        | A1   |

### Arduino Uno (Master)
| Component         | Pin        |
|-------------------|------------|
| LCD I2C (0x27)    | SDA/SCL    |
| RTC DS3231        | SDA/SCL    |

---

## Dependencies

- `Wire.h` — I2C communication
- `LiquidCrystal_I2C.h` — LCD control
- `RTClib.h` — RTC DS3231
- `Servo.h` — Servo motor control

---

## Key Design Decisions

- **The Nano is the brain** — all navigation, button debouncing, and hardware logic runs on the slave. The Uno is a pure display renderer.
- **Fixed-byte protocol** — the Nano always sends the same packet regardless of the active menu case, keeping communication simple and predictable.
- **RTC is local to the Uno** — time and date are read directly on the master for the home screen and alarm logic without involving I2C.
- **`volatile` variables on the Nano** — any variable shared between `loop()` and the `sendEvent()` ISR callback is declared `volatile` to prevent the compiler from caching stale values.

---

## Project Status

- [x] I2C master-slave communication
- [x] Menu navigation with joystick
- [x] Home screen with RTC
- [ ] Radar auto sweep
- [ ] Manual servo operation
- [ ] Logs
- [ ] Time settings
- [ ] Alarm
