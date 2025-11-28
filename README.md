# RC Car Steering and Motor Controller

This is an ESP32-based HTTPS WebSocket RC controller for steering and motor throttle (one-direction) with an adaptive, responsive web UI that supports tilt control on iOS Safari.

Features
- Steering via PWM servo (steering wheel/tilt mapping).
- Motor throttle via PWM (single direction: forward only). Acceleration is ramped; release coasts down; handbrake instantly zeroes the duty.
- HTTPS + WebSocket secure server with multiple clients.
- Web UI with 3-column landscape layout: left — steering, center — motor power meter, right — circular throttle buttons.
- iOS Safari DeviceOrientation permission flow (tap Zero Gyro button to grant permission and calibrate).

Files of Interest
- `src/main.cpp` — Main program: HTTPS server setup, servo and motor PWM handling, WebSocket handlers.
- `include/web_ui.h` — HTML/CSS/JS embedded asset served by the board. This file contains the complete web UI.
- `include/cert_der.h`, `include/key_der.h` — Self-signed cert and key used for HTTPS server. You may replace these with your own.

Wiring
- Steering servo signal pin: `servoPin` in `src/main.cpp` (example: GPIO 19)
- Motor ESC / driver PWM pin: `motorPwmPin` in `src/main.cpp` (example: GPIO 18)
- Power: Follow ESC / servo power best practices and ensure the ESP32 ground is common.

IMPORTANT SAFETY NOTE
- The motor driver (ESC) and servo can move mechanical parts at high speed. Do NOT power the motor during setup and testing until the ESC is calibrated and the props/wheels are removed.
- The code only provides forward throttle. Physically isolate the vehicle while testing.

PlatformIO Build and Upload
1. Make sure you have PlatformIO installed (VSCode recommended).
2. Connect your ESP32 board and find the upload port (e.g., `/dev/cu.usbserial-xxx`).
3. Build and upload with:

```bash
platformio run --target upload --upload-port /dev/cu.usbserial-110
```

4. Monitor serial output to see the AP IP and WebSocket status:

```bash
platformio device monitor --port /dev/cu.usbserial-110
```

Configuration
- WiFi AP: `ssid` and `password` constants at the top of `src/main.cpp` let you change the soft AP credentials. Use your phone/tablet to connect to this AP.
- Servo limits (and tilt mapping): You can tune `servoMin`, `servoMax`, and `tiltMin`/`tiltMax` to map physical steering to phone tilt range.
- Motor ramping: `motorAccelPerMs` and `motorDecelPerMs` constants control acceleration and deceleration (duty change per millisecond).

Web UI Usage
- Open the browser (Safari recommended for iOS tilt) and connect to `https://<ESP32 AP IP>/`.
- The UI has a three-column landscape layout:
  - Left: Steering 'wheel' and slider; the arrow rotates to indicate steering position.
  - Center: Motor power meter — vertical filling bar (gold on dark-blue background).
  - Right: Gas (blue) and Brake (red) circular buttons.
- Tap the `Zero Gyro` button to grant motion permissions on iOS Safari and calibrate the current wheel position. This must be a user gesture for permission.
- Gas: press & hold to ramp throttle up; release to coast down.
- Brake (handbrake): instantly sets motor duty to 0.

WebSocket messages
- Tilt slider or tilt sensor sends numbers representing tilt. The server maps tilt to servo angle.
- `sync` — Client requests the full state.
- `gas_on` / `gas_off` — Start/stop throttle.
- `handbrake` — Immediately zero motor duty.

Known Limitations & Troubleshooting
- iOS Safari does not allow programmatic fullscreen in all contexts — `document.requestFullscreen()` is restricted; use `Zero Gyro` and the PWA installation for a near-fullscreen experience.
- If the servo doesn't respond to tilt: verify pin mapping and that `ledcAttachPin()` is tied to the right channel.
- If motor doesn't spin: ensure ESC calibration and correct power wiring.
- WebSocket issues: use an `https` endpoint; self-signed certs will emit warnings. Use a device browser with the self-signed cert allowed or replace with proper cert.

Licensing & Acknowledgements
- This project is permissively licensed. Modify as needed for your own setup.

Enjoy your RC controller! Please test safely and keep fingers, pets, and bystanders clear while testing the motor.
