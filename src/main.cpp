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
#include "web_ui.h"

using namespace httpsserver;

// ====== User settings ======
const char *ssid = "MyESP32_AP";
const char *password = "esp32password";

const int servoPin = 19;      // GPIO connected to servo signal
const int servoMin = 0;       // Servo angle minimum
const int servoMax = 180;     // Servo angle maximum
const float tiltMin = -45.0f; // phone tilt min (degrees)
const float tiltMax = +45.0f; // phone tilt max

const int motorPwmPin = 18;   // GPIO connected to ESC / motor driver input

constexpr uint8_t MAX_WS_CLIENTS = 4;

// ====== Servo PWM config ======
constexpr uint8_t servoChannel = 0;
constexpr uint32_t servoFreq = 50;          // 50 Hz for standard servos
constexpr uint8_t servoResolution = 15;     // 15-bit resolution
constexpr int servoPulseMinUs = 500;        // Minimum pulse width
constexpr int servoPulseMaxUs = 2400;       // Maximum pulse width
constexpr uint32_t servoPeriodUs = 20000;   // 20 ms period at 50 Hz

// ====== Motor PWM config ======
constexpr uint8_t motorChannel = 1;
constexpr uint32_t motorFreq = 20000;       // 20 kHz to keep motor drive quiet
constexpr uint8_t motorResolution = 12;     // 12-bit resolution for duty control
constexpr float motorDutyMax = 1.0f;
constexpr float motorAccelPerMs = 1.0f / 600.0f; // reach full throttle in ~0.6s
constexpr float motorDecelPerMs = 1.0f / 900.0f; // coast down a bit slower
constexpr uint32_t motorUpdateIntervalMs = 20;

// ====== Globals ======
int currentAngle = 90;   // start at center
float currentTilt = 0.0; // track the last requested tilt
float motorDuty = 0.0f;
bool gasPressed = false;
unsigned long lastMotorUpdateMs = 0;
float lastBroadcastMotorDuty = -1.0f;

SSLCert cert(serverCertDer, serverCertDerLen, serverKeyDer, serverKeyDerLen);
HTTPSServer secureServer(&cert, 443, MAX_WS_CLIENTS);

void handleRoot(HTTPRequest *req, HTTPResponse *res);
void handle404(HTTPRequest *req, HTTPResponse *res);
void updateMotorControl();
void writeMotorDuty(float duty);
void applyHandbrake();
void broadcastState();

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

void writeMotorDuty(float duty) {
  if (duty < 0.0f) duty = 0.0f;
  if (duty > motorDutyMax) duty = motorDutyMax;
  const uint32_t maxDuty = (1u << motorResolution) - 1u;
  const uint32_t pwmValue = static_cast<uint32_t>(duty * maxDuty + 0.5f);
  ledcWrite(motorChannel, pwmValue);
}

void applyHandbrake() {
  gasPressed = false;
  motorDuty = 0.0f;
  writeMotorDuty(motorDuty);
  lastBroadcastMotorDuty = motorDuty;
  broadcastState();
}

void updateMotorControl() {
  const unsigned long now = millis();
  const unsigned long elapsed = now - lastMotorUpdateMs;
  if (elapsed < motorUpdateIntervalMs) return;
  lastMotorUpdateMs = now;

  const float ratePerMs = gasPressed ? motorAccelPerMs : -motorDecelPerMs;
  float newDuty = motorDuty + ratePerMs * static_cast<float>(elapsed);
  if (newDuty < 0.0f) newDuty = 0.0f;
  if (newDuty > motorDutyMax) newDuty = motorDutyMax;

  if (fabsf(newDuty - motorDuty) < 0.0001f) return;
  motorDuty = newDuty;
  writeMotorDuty(motorDuty);

  if (fabsf(motorDuty - lastBroadcastMotorDuty) >= 0.01f) {
    lastBroadcastMotorDuty = motorDuty;
    broadcastState();
  }
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

  ledcSetup(motorChannel, motorFreq, motorResolution);
  ledcAttachPin(motorPwmPin, motorChannel);
  writeMotorDuty(0.0f);
  lastMotorUpdateMs = millis();

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
  updateMotorControl();
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
  res->println(WEB_UI_HTML);
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
  snprintf(payload, sizeof(payload), "{\"angle\":%d,\"tilt\":%.2f,\"motorDuty\":%.3f,\"gas\":%s}", currentAngle, currentTilt, motorDuty, gasPressed ? "true" : "false");
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

  if (message == "gas_on") {
    if (!gasPressed) {
      gasPressed = true;
      broadcastState();
    }
    return;
  }

  if (message == "gas_off") {
    if (gasPressed) {
      gasPressed = false;
      broadcastState();
    }
    return;
  }

  if (message == "handbrake") {
    applyHandbrake();
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
