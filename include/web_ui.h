#pragma once

static const char MANIFEST_JSON[] = R"JSON({
  "name": "遙控車控制器",
  "short_name": "遙控車",
  "description": "遙控車操作介面",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#071428",
  "theme_color": "#071428",
  "orientation": "landscape",
  "icons": [
    {
      "src": "/icon-192.png",
      "sizes": "192x192",
      "type": "image/png"
    },
    {
      "src": "/icon-512.png",
      "sizes": "512x512",
      "type": "image/png"
    }
  ]
})JSON";

static const char WEB_UI_HTML[] = R"HTMLDOC(
<!DOCTYPE html>
<html lang="zh-Hant-TW">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no, viewport-fit=cover">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <meta name="apple-mobile-web-app-title" content="遙控車控制器">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="theme-color" content="#071428">
  <link rel="manifest" href="/manifest.json">
  <link rel="apple-touch-icon" href="/icon-192.png">
  <title>遙控車控制器</title>
  <style>
    :root {
      color-scheme: dark;
      --card-bg: rgba(16, 24, 39, 0.8);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #4fc3f7;
      --accent-strong: #00bcd4;
      --warning: #ffb74d;
    }

    * {
      box-sizing: border-box;
      -webkit-user-select: none;
      user-select: none;
      -webkit-touch-callout: none;
    }

    html, body {
      width: 100%;
      height: 100%;
      overflow: hidden;
      overscroll-behavior: none;
      touch-action: manipulation;
    }

    body {
      margin: 0;
      font-family: 'SF Pro Display', 'Inter', Arial, sans-serif;
      background: #071428;
      color: #f6f9ff;
      min-height: 100dvh;
      height: 100dvh;
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
        height: 100dvh;
        border-radius: 0;
        border: none;
        padding: 12px;
        gap: 10px;
      }
    }

    .control-grid {
      display: flex;
      gap: 24px;
      align-items: stretch;
      flex: 1 1 0;
      min-height: 0;
    }

    .column {
      flex: 1;
      display: flex;
      flex-direction: column;
      gap: 24px;
      min-height: 0;
    }

    .throttle-column {
      flex: 0 0 clamp(200px, 31vw, 360px);
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
      gap: clamp(10px, 2vh, 20px);
      align-items: stretch;
      width: min(260px, 100%);
    }

    .pedal-button {
      width: 100%;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: clamp(0.85rem, 2.1vh, 1.1rem);
      padding: 0 14px;
      border: 2px solid rgba(255, 255, 255, 0.25);
      color: #fff;
      font-weight: 600;
      border-radius: 14px;
      text-transform: uppercase;
      letter-spacing: 0.06em;
    }

    .gas-btn {
      background: #2196f3;
      height: clamp(74px, 20vh, 140px);
    }

    .brake-btn {
      background: #f44336;
      height: clamp(48px, 11vh, 76px);
    }

    .pedal-button:active, .pedal-button.active {
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
      min-height: 0;
    }

    .steering-value {
      font-size: 1.2rem;
      font-weight: 600;
    }

    .steering-wheel {
      width: clamp(96px, 26vh, 140px);
      height: clamp(96px, 26vh, 140px);
      position: relative; /* ensure arrow is positioned relative to the wheel */
      background: #041026;
      border-radius: 50%;
      border: 1px solid rgba(255,255,255,0.06);
    }

    .steering-arrow {
      position: absolute;
      left: 50%;
      top: 50%;
      width: 16px;
      height: 56px;
      transform-origin: 50% 50%;
      transform: translate(-50%, -50%) rotate(var(--angle, 0deg));
    }

    .steering-arrow::before {
      content: "";
      position: absolute;
      left: 50%;
      bottom: 0;
      transform: translateX(-50%);
      width: 4px;
      height: 40px;
      border-radius: 999px;
      background: #4fc3f7;
      box-shadow: 0 0 10px rgba(79, 195, 247, 0.5);
    }

    .steering-arrow::after {
      content: "";
      position: absolute;
      left: 50%;
      top: 0;
      transform: translateX(-50%);
      width: 0;
      height: 0;
      border-left: 10px solid transparent;
      border-right: 10px solid transparent;
      border-bottom: 16px solid #4fc3f7;
    }

    #tiltSlider {
      width: 100%;
      appearance: none;
      height: 18px;
      border-radius: 999px;
      background: rgba(255, 255, 255, 0.15);
      outline: none;
    }

    #tiltSlider::-webkit-slider-thumb {
      appearance: none;
      width: 36px;
      height: 36px;
      border-radius: 50%;
      background: var(--accent);
      box-shadow: 0 6px 20px rgba(79, 195, 247, 0.45);
      border: 2px solid rgba(255, 255, 255, 0.5);
    }

    #tiltSlider::-moz-range-thumb {
      width: 36px;
      height: 36px;
      border-radius: 50%;
      background: var(--accent);
      border: none;
    }

    instructions {
      font-size: 0.5rem;
      color: rgba(255, 255, 255, 0.7);
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
      width: clamp(24px, 4.5vw, 40px);
      height: clamp(110px, 44vh, 210px);
      border-radius: 20px;
      background: #0d47a1;
      overflow: hidden;
      border: 1px solid rgba(255, 255, 255, 0.2);
      margin: 0 auto;
      position: relative;
    }

    @media (orientation: landscape) and (max-height: 430px) {
      body {
        padding: env(safe-area-inset-top) env(safe-area-inset-right) env(safe-area-inset-bottom) env(safe-area-inset-left);
      }

      main {
        padding: 10px 12px;
        gap: 8px;
      }

      header {
        display: grid;
        grid-template-columns: 1fr auto;
        align-items: center;
        gap: 8px 12px;
      }

      h1 { font-size: 1rem; }

      #status { font-size: 0.78rem; }

      }
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
      .pedal-button {
        font-size: 0.8rem;
      }

      .steering-wheel {
        width: 120px;
        height: 120px;
      }

      .steering-arrow {
        width: 14px;
        height: 44px;
      }

      .steering-arrow::before {
        height: 30px;
      }

      .steering-arrow::after {
        border-left-width: 8px;
        border-right-width: 8px;
        border-bottom-width: 14px;
      }
    }

    @media (max-width: 600px) {
      main { border-radius: 16px; }
      button { flex: 1; text-align: center; }
    }

    #orientationGuard {
      position: fixed;
      inset: 0;
      z-index: 9999;
      display: none;
      align-items: center;
      justify-content: center;
      text-align: center;
      padding: 24px;
      background: rgba(2, 8, 20, 0.96);
      color: #f6f9ff;
      font-size: 1.05rem;
      line-height: 1.5;
      letter-spacing: 0.02em;
    }

    #orientationGuard.show {
      display: flex;
    }
  </style>
</head>
<body>
  <div id="orientationGuard">請將手機旋轉為橫向，以使用遙控介面。</div>
  <main>
    <header>
      <div>
        <h1>遙控車控制器</h1>
        <p id="status">連線中…</p>
      </div>
      <div class="button-row">
        <button id="gyroButton" type="button">陀螺儀歸零</button>
        <button id="headlightButton" type="button">頭燈</button>
      </div>
      <p id="gyroStatus">點選「陀螺儀歸零」以授權動作感測，並校正目前方向盤位置。</p>
    </header>

    <div class="control-grid">
      <div class="column steering-column">
        <section class="card steering-card">
          <div class="steering-wheel" id="steeringWheel">
            <div class="steering-arrow" id="steeringArrow"></div>
          </div>
          <div class="steering-value" id="angleDisplay">方向：90°</div>
          <input type="range" min="-45" max="45" step="0.1" value="0" id="tiltSlider">
          <p>可拖曳滑桿或在橫向模式下傾斜手機轉向。</p>
        </section>
      </div>

      <div class="column throttle-column">
        <section class="card throttle-card">
          <div class="throttle-title">油門 / 煞車</div>
          <div class="throttle-buttons">
            <button class="pedal-button gas-btn" id="gasButton" type="button">油門</button>
            <button class="pedal-button brake-btn" id="handbrakeButton" type="button">煞車</button>
          </div>
          <p class="instructions">按住油門加速</p>
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
    const gyroButton = document.getElementById('gyroButton');
    const gyroStatusEl = document.getElementById('gyroStatus');
    const headlightButton = document.getElementById('headlightButton');
    let ws;
    let gasHeld = false;
    let gyroEnabled = false;
    let gyroRequestInFlight = false;
    let lastTiltSent = parseFloat(slider.value);
    let lastRawWheel = 0;
    let gyroZeroOffset = 0;
    let headlightOn = false;
    const orientationGuard = document.getElementById('orientationGuard');
    const isStandalonePwa = window.matchMedia('(display-mode: standalone)').matches || window.navigator.standalone === true;

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

    document.addEventListener('contextmenu', (event) => {
      event.preventDefault();
    });

    document.addEventListener('gesturestart', (event) => {
      event.preventDefault();
    });

    document.addEventListener('touchmove', (event) => {
      if (!event.target.closest('#tiltSlider')) {
        event.preventDefault();
      }
    }, { passive: false });

    document.addEventListener('dblclick', (event) => {
      event.preventDefault();
    });

    const isLandscapeOrientation = () => {
      if (window.screen?.orientation && typeof window.screen.orientation.angle === 'number') {
        return Math.abs(window.screen.orientation.angle) === 90;
      }
      if (typeof window.orientation === 'number') {
        return Math.abs(window.orientation) === 90;
      }
      return window.innerWidth > window.innerHeight;
    };

    const updateOrientationGuard = () => {
      if (!isStandalonePwa) {
        orientationGuard.classList.remove('show');
        return;
      }
      orientationGuard.classList.toggle('show', !isLandscapeOrientation());
    };

    const requestLandscapeLock = async () => {
      if (!isStandalonePwa) return;
      if (window.screen?.orientation?.lock) {
        try {
          await window.screen.orientation.lock('landscape');
        } catch (_) {
        }
      }
      updateOrientationGuard();
    };

    window.addEventListener('orientationchange', () => {
      updateOrientationGuard();
      requestLandscapeLock();
    });

    window.addEventListener('resize', updateOrientationGuard);

    document.addEventListener('visibilitychange', () => {
      if (!document.hidden) {
        requestLandscapeLock();
      }
    });

    document.addEventListener('pointerdown', requestLandscapeLock, { once: true });

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
      gyroStatusEl.textContent = '陀螺儀已啟用，可隨時再次點選「陀螺儀歸零」重新校正。';
    };

    const ensureGyroAccess = () => {
      if (gyroEnabled) return Promise.resolve();
      if (gyroRequestInFlight) return Promise.reject(new Error('pending'));
      gyroRequestInFlight = true;
      gyroStatusEl.textContent = '正在要求動作感測權限…';

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
      gyroStatusEl.textContent = '已完成方向置中，請像轉方向盤一樣旋轉手機。';
    };

    const handleZeroButton = () => {
      ensureGyroAccess()
        .then(() => {
          zeroGyro();
        })
        .catch((err) => {
          console.error('Gyro access error', err);
          if (err?.message === 'unsupported') {
            gyroStatusEl.textContent = '裝置不支援方向感測，請改用滑桿控制。';
          } else if (err?.message === 'denied') {
            gyroStatusEl.textContent = '已拒絕動作權限，請到 Safari 設定開啟「動作與方向存取」。';
          } else if (err?.message === 'pending') {
            gyroStatusEl.textContent = '權限要求進行中…';
          } else {
            gyroStatusEl.textContent = '無法啟用陀螺儀，請再試一次。';
          }
        });
    };

    function connectWs() {
      const proto = location.protocol === 'https:' ? 'wss://' : 'ws://';
      ws = new WebSocket(proto + location.host + '/ws');

      ws.onopen = () => {
        statusEl.textContent = '已連線';
        sendCommand('sync');
      };

      ws.onclose = () => {
        statusEl.textContent = '已斷線，重新連線中…';
        setTimeout(connectWs, 2000);
      };

      ws.onerror = () => {
        statusEl.textContent = 'WebSocket 錯誤';
        ws.close();
      };

      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          if (typeof data.angle === 'number') {
            angleEl.textContent = `方向：${data.angle}°`;
            const normalizedTilt = clamp(data.angle - 90, -45, 45);
            setSteeringIndicator(normalizedTilt);
          }
          if (typeof data.tilt === 'number' && slider !== document.activeElement) {
            slider.value = data.tilt;
            lastTiltSent = parseFloat(data.tilt);
            setSteeringIndicator(lastTiltSent);
          }
          if (typeof data.gas === 'boolean') {
            gasButton.classList.toggle('active', data.gas);
          }
          if (typeof data.headlight === 'boolean') {
            headlightOn = data.headlight;
            headlightButton.classList.toggle('active', headlightOn);
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

    headlightButton.addEventListener('click', () => {
      headlightOn = !headlightOn;
      headlightButton.classList.toggle('active', headlightOn);
      sendCommand(headlightOn ? 'headlight_on' : 'headlight_off');
    });

    requestLandscapeLock();
    updateOrientationGuard();
    connectWs();
  </script>
</body>
</html>
)HTMLDOC";
