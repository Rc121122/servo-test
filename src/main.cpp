#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

// ====== User settings ======
const char* ssid     = "MyESP32_AP";
const char* password = "esp32password";

const int servoPin = 18;       // GPIO connected to servo signal
const int servoMin = 0;        // Servo angle minimum
const int servoMax = 180;      // Servo angle maximum
const float tiltMin = -45.0;   // phone tilt min (degrees)
const float tiltMax = +45.0;   // phone tilt max

// ====== Globals ======
AsyncWebServer server(80);
Servo steeringServo;
int currentAngle = 90;  // start at center

// ====== Utility: map tilt to angle ======
int mapTiltToAngle(float tilt) {
  // clamp
  if (tilt < tiltMin) tilt = tiltMin;
  if (tilt > tiltMax) tilt = tiltMax;
  // map
  float norm = (tilt - tiltMin) / (tiltMax - tiltMin);  // 0.0 to 1.0
  int angle = servoMin + (int)(norm * (servoMax - servoMin));
  return angle;
}

void setup(){
  Serial.begin(115200);
  Serial.println("Starting ESP32 Steering Web Server...");

  // Setup servo
  steeringServo.attach(servoPin);
  steeringServo.write(currentAngle);

  // Setup WiFi as Access Point
  Serial.print("Setting up AP: "); Serial.println(ssid);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: "); Serial.println(IP);

  // Serve HTML page at root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Steering Control</title>
      </head>
      <body style="text-align:center">
        <h1>RC Car Steering</h1>
        <p>Use the slider to steer left/right.</p>
        <input type="range" min="-45" max="45" value="0" id="tiltSlider" style="width: 80%;">
        <p id="angleDisplay">Angle: 90</p>
        <script>
          const slider = document.getElementById('tiltSlider');
          const display = document.getElementById('angleDisplay');
          slider.addEventListener('input', function() {
            let tilt = parseFloat(slider.value);
            fetch('/steer?tilt=' + tilt)
              .then(response => response.text())
              .then(angle => {
                display.textContent = 'Angle: ' + angle;
              })
              .catch(err => console.log(err));
          });
        </script>
      </body>
      </html>
    )rawliteral");
  });

  // Handle steering
  server.on("/steer", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("tilt")) {
      String tiltStr = request->getParam("tilt")->value();
      float tilt = tiltStr.toFloat();
      int angle = mapTiltToAngle(tilt);
      steeringServo.write(angle);
      currentAngle = angle;
      Serial.printf("Tilt: %.2f deg → Angle: %d\n", tilt, angle);
      request->send(200, "text/plain", String(angle));
    } else {
      request->send(400, "text/plain", "Missing tilt param");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // nothing special to do: everything handled in callbacks
}
