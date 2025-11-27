#include <Arduino.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <SSLCert.hpp>
#include <WebsocketHandler.hpp>
#include <WebsocketNode.hpp>

#include "cert_der.h"
#include "key_der.h"

using namespace httpsserver;

// ====== User settings ======
const char *ssid = "MyESP32_AP";
const char *password = "esp32password";

const int servoPin = 18;      // GPIO connected to servo signal
const int servoMin = 0;       // Servo angle minimum
const int servoMax = 180;     // Servo angle maximum
const float tiltMin = -45.0f; // phone tilt min (degrees)
const float tiltMax = +45.0f; // phone tilt max

constexpr uint8_t MAX_WS_CLIENTS = 4;

// ====== Servo PWM config ======
constexpr uint8_t servoChannel = 0;
constexpr uint32_t servoFreq = 50;          // 50 Hz for standard servos
constexpr uint8_t servoResolution = 15;     // 15-bit resolution
constexpr int servoPulseMinUs = 500;        // Minimum pulse width
constexpr int servoPulseMaxUs = 2400;       // Maximum pulse width
constexpr uint32_t servoPeriodUs = 20000;   // 20 ms period at 50 Hz

// ====== Globals ======
int currentAngle = 90;   // start at center
float currentTilt = 0.0; // track the last requested tilt

SSLCert cert(serverCertDer, serverCertDerLen, serverKeyDer, serverKeyDerLen);
HTTPSServer secureServer(&cert, 443, MAX_WS_CLIENTS);

void handleRoot(HTTPRequest *req, HTTPResponse *res);
void handle404(HTTPRequest *req, HTTPResponse *res);

class SteeringWebsocket : public WebsocketHandler {
public:
  static WebsocketHandler *create();
  void onMessage(WebsocketInputStreambuf *input) override;
  void onClose() override;
  void sendState();
};

SteeringWebsocket *wsClients[MAX_WS_CLIENTS] = {nullptr};

int mapTiltToAngle(float tilt) {
  if (tilt < tiltMin) tilt = tiltMin;
  if (tilt > tiltMax) tilt = tiltMax;
  float norm = (tilt - tiltMin) / (tiltMax - tiltMin);
  return servoMin + static_cast<int>(norm * (servoMax - servoMin));
}

void writeServoAngle(int angle) {
  if (angle < servoMin) angle = servoMin;
  if (angle > servoMax) angle = servoMax;
  const uint32_t maxDuty = (1u << servoResolution) - 1u;
  const int pulseUs = servoPulseMinUs + (angle - servoMin) * (servoPulseMaxUs - servoPulseMinUs) / (servoMax - servoMin);
  const uint32_t duty = (static_cast<uint64_t>(pulseUs) * maxDuty) / servoPeriodUs;
  ledcWrite(servoChannel, duty);
}

void broadcastState() {
  for (uint8_t i = 0; i < MAX_WS_CLIENTS; ++i) {
    if (wsClients[i] != nullptr) {
      wsClients[i]->sendState();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Steering HTTPS server...");

  ledcSetup(servoChannel, servoFreq, servoResolution);
  ledcAttachPin(servoPin, servoChannel);
  writeServoAngle(currentAngle);

  Serial.print("Setting up AP: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  ResourceNode *rootNode = new ResourceNode("/", "GET", &handleRoot);
  secureServer.registerNode(rootNode);

  WebsocketNode *wsNode = new WebsocketNode("/ws", &SteeringWebsocket::create);
  secureServer.registerNode(wsNode);

  ResourceNode *notFoundNode = new ResourceNode("", "GET", &handle404);
  secureServer.setDefaultNode(notFoundNode);

  Serial.println("Starting HTTPS server...");
  secureServer.start();
  if (secureServer.isRunning()) {
    Serial.print("Server ready at https://");
    Serial.println(ip);
  } else {
    Serial.println("Failed to start HTTPS server");
  }
}

void loop() {
  secureServer.loop();
  delay(1);
}

void handle404(HTTPRequest *req, HTTPResponse *res) {
  req->discardRequestBody();
  res->setStatusCode(404);
  res->setStatusText("Not Found");
  res->setHeader("Content-Type", "text/html");
  res->println("<!DOCTYPE html>");
  res->println("<html><head><title>404 Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1></body></html>");
}

void handleRoot(HTTPRequest *req, HTTPResponse *res) {
  req->discardRequestBody();
  res->setHeader("Content-Type", "text/html");
  res->println(R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Steering Control</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 40px; background: #f4f4f4; }
    h1 { margin-bottom: 16px; }
    #status { font-size: 0.9rem; color: #555; }
    #tiltSlider { width: 80%; max-width: 400px; margin: 24px auto; display: block; }
    #angleDisplay { font-size: 1.2rem; margin-top: 12px; }
  </style>
</head>
<body>
  <h1>RC Car Steering</h1>
  <p id="status">Connecting…</p>
  <input type="range" min="-45" max="45" step="0.1" value="0" id="tiltSlider">
  <p id="angleDisplay">Angle: 90</p>
  <script>
    const slider = document.getElementById('tiltSlider');
    const statusEl = document.getElementById('status');
    const angleEl = document.getElementById('angleDisplay');
    let ws;

    function connectWs() {
      const proto = location.protocol === 'https:' ? 'wss://' : 'ws://';
      ws = new WebSocket(proto + location.host + '/ws');

      ws.onopen = () => {
        statusEl.textContent = 'Connected';
        ws.send('sync');
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
            angleEl.textContent = `Angle: ${data.angle}`;
          }
          if (typeof data.tilt === 'number' && slider !== document.activeElement) {
            slider.value = data.tilt;
          }
        } catch (err) {
          console.error('Invalid payload', err);
        }
      };
    }

    slider.addEventListener('input', () => {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(slider.value);
      }
    });

    connectWs();
  </script>
</body>
</html>
)rawliteral");
}

WebsocketHandler *SteeringWebsocket::create() {
  SteeringWebsocket *handler = new SteeringWebsocket();
  for (uint8_t i = 0; i < MAX_WS_CLIENTS; ++i) {
    if (wsClients[i] == nullptr) {
      wsClients[i] = handler;
      break;
    }
  }
  return handler;
}

void SteeringWebsocket::onClose() {
  for (uint8_t i = 0; i < MAX_WS_CLIENTS; ++i) {
    if (wsClients[i] == this) {
      wsClients[i] = nullptr;
    }
  }
}

void SteeringWebsocket::sendState() {
  char payload[64];
  snprintf(payload, sizeof(payload), "{\"angle\":%d,\"tilt\":%.2f}", currentAngle, currentTilt);
  send(std::string(payload), WebsocketHandler::SEND_TYPE_TEXT);
}

void SteeringWebsocket::onMessage(WebsocketInputStreambuf *input) {
  std::ostringstream ss;
  ss << input;
  std::string message = ss.str();

  if (message == "sync") {
    sendState();
    return;
  }

  char *endPtr = nullptr;
  float tilt = strtof(message.c_str(), &endPtr);
  if (endPtr == message.c_str() || !isfinite(tilt)) {
    send("{\"error\":\"invalid_input\"}", WebsocketHandler::SEND_TYPE_TEXT);
    return;
  }

  currentTilt = tilt;
  int angle = mapTiltToAngle(tilt);
  if (angle != currentAngle) {
    writeServoAngle(angle);
    currentAngle = angle;
    Serial.printf("Tilt: %.2f deg -> Angle: %d\n", currentTilt, currentAngle);
  }

  broadcastState();
}
