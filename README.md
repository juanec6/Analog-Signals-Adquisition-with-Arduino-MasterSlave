# Arduino Master-Slave Monitoring System

Monitoring system based on Arduino using I2C master-slave architecture. The slave (Nano) handles all hardware logic and menu state, while the master (Uno) manages the display and renders each screen based on what the slave reports.

---

## Architecture

```
┌──────────────────────────────┐      I2C (2 bytes)      ┌──────────────────────────────┐
│       Arduino Nano           │ ──────────────────────► │       Arduino Uno            │
│         (Slave)              │                          │         (Master)             │
│                              │                          │                              │
│  - Joystick navigation       │                          │  - LCD 16x2 (0x27)           │
│  - Enter / Back buttons      │                          │  - RTC DS3231                │
│  - Menu state management     │                          │  - Menu rendering            │
│  - menuLevel & currentOption │                          │  - Case-based display logic  │
└──────────────────────────────┘                          └──────────────────────────────┘
```

---

## I2C Protocol

The Nano sends a 2-byte packet to the Uno on every request:

| Byte | Content         | Type    | Description                         |
|------|-----------------|---------|-------------------------------------|
| 0    | `menuLevel`     | uint8_t | Current screen (0 = home, 1 = menu) |
| 1    | `currentOption` | int8_t  | Selected menu item (0–4)            |

---

## Menu Structure

```
[menuLevel 0] Home — Time and date from RTC
      ↓ ENTER (SW_PIN)
[menuLevel 1] Menu — Joystick navigates up/down between options:
                      0. Radar
                      1. Logs
                      2. Microphone
                      3. Humidity
                      4. Temperature
      ↑ BACK — returns to home screen
```

---

## Hardware

### Arduino Nano (Slave)
| Component       | Pin |
|-----------------|-----|
| Enter button    | D3  |
| Back button     | D6  |
| Red LED         | D5  |
| Green LED       | D4  |
| Joystick X      | A0  |
| Joystick Y      | A1  |

### Arduino Uno (Master)
| Component       | Connection  |
|-----------------|-------------|
| LCD I2C (0x27)  | SDA / SCL   |
| RTC DS3231      | SDA / SCL   |

---

## Dependencies

- `Wire.h` — I2C communication
- `LiquidCrystal_I2C.h` — LCD 16x2 control
- `RTClib.h` — RTC DS3231

---

## Key Design Decisions

- **Nano is the brain** — all navigation, button debouncing, and joystick logic runs on the slave. The Uno is a pure display renderer.
- **2-byte protocol** — only `menuLevel` and `currentOption` are sent, keeping communication minimal for the current stage.
- **RTC is local to the Uno** — time and date are read directly on the master for the home screen, no I2C overhead needed for that case.
- **`volatile` on shared variables** — `menuLevel` and `currentOption` are declared `volatile` on the Nano because they are written in `loop()` and read inside the `sendEvent()` ISR callback.
- **Joystick deadzone** — set to 250 (out of 512) to avoid accidental navigation from a resting joystick.
- **`int8_t` for `currentOption`** — used instead of `uint8_t` to correctly handle the wrap-around check `if(currentOption < 0)` when navigating up from the first item.
- **lcd.clear() on state change** — the master only clears the display when `menuLevel` or `currentOption` changes, avoiding constant flicker.

---

## Project Status

- [x] I2C master-slave communication
- [x] Menu navigation with joystick
- [x] Home screen with RTC (time and date)
- [x] Menu rendering with scroll preview
- [ ] Radar — servo sweep + ultrasonic distance
- [ ] Logs
- [ ] Microphone
- [ ] Humidity
- [ ] Temperature
