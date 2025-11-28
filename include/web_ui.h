#pragma once

static const char WEB_UI_HTML[] = R"HTMLDOC(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <title>RC Car Control</title>
  <style>
    :root {
      color-scheme: dark;
      --card-bg: rgba(16, 24, 39, 0.8);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #4fc3f7;
      --accent-strong: #00bcd4;
      --warning: #ffb74d;
    }

    * { box-sizing: border-box; }

    body {
      margin: 0;
      font-family: 'SF Pro Display', 'Inter', Arial, sans-serif;
      background: #071428;
      color: #f6f9ff;
      min-height: 100vh;
      display: flex;
      align-items: stretch;
      justify-content: center;
      padding: clamp(16px, 4vw, 32px);
    }

    @media (orientation: landscape) {
      body {
        padding: 0;
      }
    }

    main {
      width: min(1100px, 100%);
      backdrop-filter: blur(16px);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      background: rgba(3, 7, 18, 0.65);
      box-shadow: 0 20px 60px rgba(2, 6, 23, 0.75);
      padding: clamp(20px, 4vw, 36px);
      display: flex;
      flex-direction: column;
      gap: 24px;
      align-items: stretch;
    }

    @media (orientation: landscape) {
      main {
        width: 100vw;
        height: 100vh;
        border-radius: 0;
        border: none;
        padding: 20px;
        gap: 16px;
      }
    }

    .control-grid {
      display: flex;
      gap: 24px;
      align-items: stretch;
      flex: 1 1 0;
    }

    .column {
      flex: 1;
      display: flex;
      flex-direction: column;
      gap: 24px;
    }

    .mid-column {
      flex: 0 0 200px;
      justify-content: center;
    }

    .throttle-column {
      flex: 0 0 320px;
    }

    h1 {
      font-size: clamp(1.2rem, 3vw, 1.8rem);
      margin: 0;
    }

    #status {
      font-size: 0.9rem;
      color: var(--warning);
    }

    .button-row {
      display: flex;
      flex-wrap: wrap;
      gap: 12px;
    }

    button {
      font-size: 1rem;
      padding: 12px 24px;
      border-radius: 999px;
      border: 1px solid transparent;
      color: #041021;
      background: var(--accent);
      cursor: pointer;
      transition: transform 0.2s ease, box-shadow 0.2s ease, background 0.2s ease;
    }

    button:hover {
      transform: translateY(-1px);
      box-shadow: 0 10px 20px rgba(79, 195, 247, 0.25);
    }

    button:active, button.active {
      background: var(--accent-strong);
    }

    button:disabled {
      opacity: 0.4;
      cursor: not-allowed;
    }

    .throttle-card {
      align-items: center;
      text-align: center;
      justify-content: center;
    }

    .throttle-title {
      font-size: 1.1rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: rgba(255, 255, 255, 0.7);
    }

    .throttle-buttons {
      display: flex;
      flex-direction: column;
      gap: 20px;
      align-items: center;
    }

    .circle-button {
      width: 120px;
      height: 120px;
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 1.1rem;
      padding: 0;
      border: 2px solid rgba(255, 255, 255, 0.25);
      color: #fff;
      font-weight: 600;
    }

    .gas-btn {
      background: #2196f3;
    }

    .brake-btn {
      background: #f44336;
    }

    .circle-button:active, .circle-button.active {
      opacity: 0.8;
    }

    section.card {
      border-radius: 20px;
      border: 1px solid var(--card-border);
      background: var(--card-bg);
      padding: clamp(16px, 3vw, 28px);
      display: flex;
      flex-direction: column;
      flex: 1;
    }

    .steering-value {
      font-size: 1.2rem;
      font-weight: 600;
    }

    .steering-wheel {
      width: 120px;
      height: 120px;
      position: relative; /* ensure arrow is positioned relative to the wheel */
      background: #041026;
      border-radius: 50%;
      border: 1px solid rgba(255,255,255,0.06);
    }

    .steering-arrow {
      position: absolute;
      left: 50%;
      top: 50%;
      width: 0;
      height: 0;
      border-left: 12px solid transparent;
      border-right: 12px solid transparent;
      border-bottom: 24px solid #4fc3f7;
      transform-origin: 50% 50%;
      transform: translate(-50%, -50%) rotate(var(--angle, 0deg));
    }

    #tiltSlider {
      width: 100%;
      appearance: none;
      height: 12px;
      border-radius: 999px;
      background: rgba(255, 255, 255, 0.15);
      outline: none;
    }

    #tiltSlider::-webkit-slider-thumb {
      appearance: none;
      width: 26px;
      height: 26px;
      border-radius: 50%;
      background: var(--accent);
      box-shadow: 0 6px 20px rgba(79, 195, 247, 0.45);
      border: 2px solid rgba(255, 255, 255, 0.5);
    }

    #tiltSlider::-moz-range-thumb {
      width: 26px;
      height: 26px;
      border-radius: 50%;
      background: var(--accent);
      border: none;
    }

    section.card p {
      font-size: 0.9rem;
      margin: 8px 0;
    }

    #gyroStatus {
      font-size: 0.8rem;
      color: rgba(255, 255, 255, 0.7);
      min-height: 1.4rem;
    }

    .motor-meter {
      width: 40px;
      height: 200px;
      border-radius: 20px;
      background: #0d47a1;
      overflow: hidden;
      border: 1px solid rgba(255, 255, 255, 0.2);
      margin: 0 auto;
      position: relative;
    }

    #motorDutyBar {
      width: 100%;
      height: 0%;
      background: #ffd700;
      position: absolute;
      bottom: 0;
      transition: height 0.2s ease;
    }

    .duty-title {
      font-size: 1.1rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: rgba(255, 255, 255, 0.7);
      text-align: center;
    }

    @media (orientation: portrait) or (max-width: 900px) {
      .circle-button {
        width: 60px;
        height: 60px;
        font-size: 0.8rem;
      }

      .motor-meter {
        width: 24px;
        height: 120px;
      }

      .steering-wheel {
        width: 120px;
        height: 120px;
      }

      .steering-arrow {
        border-left-width: 8px;
        border-right-width: 8px;
        border-bottom-width: 16px;
        top: 20px;
        transform-origin: 8px 16px;
      }
    }

    @media (max-width: 600px) {
      main { border-radius: 16px; }
      button { flex: 1; text-align: center; }
    }
  </style>
</head>
<body>
  <main>
    <header>
      <div>
        <h1>RC Car Controller</h1>
        <p id="status">Connecting…</p>
      </div>
      <div class="button-row">
        <button id="gyroButton" type="button">Zero Gyro</button>
      </div>
      <p id="gyroStatus">Tap “Zero Gyro” to grant motion access and calibrate the current wheel position.</p>
    </header>

    <div class="control-grid">
      <div class="column steering-column">
        <section class="card steering-card">
          <div class="steering-wheel" id="steeringWheel">
            <div class="steering-arrow" id="steeringArrow"></div>
          </div>
          <div class="steering-value" id="angleDisplay">Steering: 90°</div>
          <input type="range" min="-45" max="45" step="0.1" value="0" id="tiltSlider">
          <p>Slide or tilt (landscape) to steer.</p>
        </section>
      </div>

      <div class="column mid-column">
        <section class="card duty-card">
          <div class="duty-title">Motor Power</div>
          <div class="motor-meter"><div id="motorDutyBar"></div></div>
          <p id="motorDisplay">0%</p>
        </section>
      </div>

      <div class="column throttle-column">
        <section class="card throttle-card">
          <div class="throttle-title">Throttle</div>
          <div class="throttle-buttons">
            <button class="circle-button gas-btn" id="gasButton" type="button">Gas</button>
            <button class="circle-button brake-btn" id="handbrakeButton" type="button">Brake</button>
          </div>
          <p>Hold Gas to accelerate. Tap Brake to cut power.</p>
        </section>
      </div>
    </div>
  </main>

  <script>
    const slider = document.getElementById('tiltSlider');
    const statusEl = document.getElementById('status');
    const angleEl = document.getElementById('angleDisplay');
    const steeringWheel = document.getElementById('steeringWheel');
    const gasButton = document.getElementById('gasButton');
    const handbrakeButton = document.getElementById('handbrakeButton');
    const motorEl = document.getElementById('motorDisplay');
    const motorDutyBar = document.getElementById('motorDutyBar');
    const gyroButton = document.getElementById('gyroButton');
    const gyroStatusEl = document.getElementById('gyroStatus');
    let ws;
    let gasHeld = false;
    let gyroEnabled = false;
    let gyroRequestInFlight = false;
    let lastTiltSent = parseFloat(slider.value);
    let lastRawWheel = 0;
    let gyroZeroOffset = 0;

    const setSteeringIndicator = (tiltDegrees) => {
      const arrow = document.getElementById('steeringArrow');
      if (arrow) arrow.style.setProperty('--angle', `${tiltDegrees * 2}deg`);
    };

    const sendCommand = (payload) => {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(payload);
      }
    };

    const clamp = (value, min, max) => Math.min(max, Math.max(min, value));

    const updateFullscreenLabel = () => {
      // Removed for iOS compatibility
    };

    const toggleFullscreen = () => {
      // Not supported on iOS Safari
    };

    // Fullscreen not used on iOS; no-op placeholder kept for compatibility.

    const isLandscapeOrientation = () => {
      if (window.screen?.orientation && typeof window.screen.orientation.angle === 'number') {
        return Math.abs(window.screen.orientation.angle) === 90;
      }
      if (typeof window.orientation === 'number') {
        return Math.abs(window.orientation) === 90;
      }
      return window.innerWidth > window.innerHeight;
    };

    const computeWheelRotation = (event) => {
      if (isLandscapeOrientation() && typeof event.beta === 'number') {
        return event.beta;
      }
      if (typeof event.gamma === 'number') {
        return event.gamma;
      }
      if (typeof event.beta === 'number') {
        return event.beta;
      }
      return 0;
    };

    const handleOrientation = (event) => {
      if (!gyroEnabled) return;
      const rawWheel = computeWheelRotation(event);
      lastRawWheel = rawWheel;
      const tilt = clamp(rawWheel - gyroZeroOffset, -45, 45);
      if (Math.abs(tilt - lastTiltSent) < 0.5) return;
      slider.value = tilt.toFixed(1);
      lastTiltSent = tilt;
      setSteeringIndicator(tilt);
      sendCommand(tilt.toFixed(2));
    };

    const startGyroStream = () => {
      if (gyroEnabled) return;
      window.addEventListener('deviceorientation', handleOrientation);
      gyroEnabled = true;
      gyroStatusEl.textContent = 'Gyro ready. Tap “Zero Gyro” any time to recenter the steering wheel.';
    };

    const ensureGyroAccess = () => {
      if (gyroEnabled) return Promise.resolve();
      if (gyroRequestInFlight) return Promise.reject(new Error('pending'));
      gyroRequestInFlight = true;
      gyroStatusEl.textContent = 'Requesting motion access…';

      return new Promise((resolve, reject) => {
        if (typeof DeviceOrientationEvent === 'undefined') {
          gyroRequestInFlight = false;
          reject(new Error('unsupported'));
          return;
        }

        const onGranted = () => {
          gyroRequestInFlight = false;
          startGyroStream();
          resolve();
        };

        const onDenied = (err) => {
          gyroRequestInFlight = false;
          reject(err);
        };

        if (typeof DeviceOrientationEvent.requestPermission === 'function') {
          DeviceOrientationEvent.requestPermission()
            .then((state) => {
              if (state === 'granted') {
                onGranted();
              } else {
                onDenied(new Error('denied'));
              }
            })
            .catch(onDenied);
        } else {
          onGranted();
        }
      });
    };

    const zeroGyro = () => {
      gyroZeroOffset = lastRawWheel;
      gyroStatusEl.textContent = 'Steering centered. Rotate the device like a wheel.';
    };

    const handleZeroButton = () => {
      ensureGyroAccess()
        .then(() => {
          zeroGyro();
        })
        .catch((err) => {
          console.error('Gyro access error', err);
          if (err?.message === 'unsupported') {
            gyroStatusEl.textContent = 'Device orientation not supported. Use the slider instead.';
          } else if (err?.message === 'denied') {
            gyroStatusEl.textContent = 'Motion access denied. Check Safari Settings ▸ Motion & Orientation.';
          } else if (err?.message === 'pending') {
            gyroStatusEl.textContent = 'Motion access already requested…';
          } else {
            gyroStatusEl.textContent = 'Unable to start gyro. Try again.';
          }
        });
    };

    function connectWs() {
      const proto = location.protocol === 'https:' ? 'wss://' : 'ws://';
      ws = new WebSocket(proto + location.host + '/ws');

      ws.onopen = () => {
        statusEl.textContent = 'Connected';
        sendCommand('sync');
      };

      ws.onclose = () => {
        statusEl.textContent = 'Disconnected, retrying…';
        setTimeout(connectWs, 2000);
      };

      ws.onerror = () => {
        statusEl.textContent = 'WebSocket error';
        ws.close();
      };

      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          if (typeof data.angle === 'number') {
            angleEl.textContent = `Steering: ${data.angle}°`;
            const normalizedTilt = clamp(data.angle - 90, -45, 45);
            setSteeringIndicator(normalizedTilt);
          }
          if (typeof data.tilt === 'number' && slider !== document.activeElement) {
            slider.value = data.tilt;
            lastTiltSent = parseFloat(data.tilt);
            setSteeringIndicator(lastTiltSent);
          }
          if (typeof data.motorDuty === 'number') {
            const pct = Math.round(data.motorDuty * 100);
            motorEl.textContent = `${pct}%`;
            motorDutyBar.style.height = `${pct}%`;
          }
          if (typeof data.gas === 'boolean') {
            gasButton.classList.toggle('active', data.gas);
          }
        } catch (err) {
          console.error('Invalid payload', err);
        }
      };
    }

    slider.addEventListener('input', () => {
      lastTiltSent = parseFloat(slider.value);
      setSteeringIndicator(lastTiltSent);
      sendCommand(slider.value);
    });

    const engageGas = () => {
      if (!gasHeld) {
        gasHeld = true;
        gasButton.classList.add('active');
        sendCommand('gas_on');
      }
    };

    const releaseGas = () => {
      if (gasHeld) {
        gasHeld = false;
        gasButton.classList.remove('active');
        sendCommand('gas_off');
      }
    };

    gasButton.addEventListener('mousedown', engageGas);
    gasButton.addEventListener('touchstart', (evt) => {
      evt.preventDefault();
      engageGas();
    }, { passive: false });

    const releaseEvents = ['mouseup', 'mouseleave', 'touchend', 'touchcancel', 'pointerup', 'pointercancel'];
    releaseEvents.forEach((eventName) => {
      window.addEventListener(eventName, releaseGas);
    });

    window.addEventListener('blur', releaseGas);

    handbrakeButton.addEventListener('click', () => {
      releaseGas();
      sendCommand('handbrake');
    });

    gyroButton.addEventListener('click', handleZeroButton);

    connectWs();
  </script>
</body>
</html>
)HTMLDOC";
