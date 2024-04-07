#include <Servo.h>
#include <WiFiS3.h>

#include <vector>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

Servo primerServo;
int primerServoPin = 2;
Servo triggerServo;
int triggerServoPin = 4;

struct Route {
  String path;
  void (*callback)();
};

class WebServer {
  private:
    WiFiServer server;
    WiFiClient client;
    std::vector<Route> routes;

    String translateStatusCode(int statusCode) {
      switch (statusCode) {
        case 200:
          return "OK";
        case 404:
          return "Not Found";
        default:
          return "Unknown";
      }
    }

    void handleNotFound() {
      send(404, "text/plain", "Not Found");
    }

  public:
    WebServer(int port) : server(port) {}
  
    void addRoute(String routePath, void (*callback)()) {
      routes.push_back({ routePath, callback });
    }

    void send(int statusCode, String contentType, String content) {
      String statusString = "HTTP/1.1 " + String(statusCode) + " " + translateStatusCode(statusCode);
      String contentTypeString = "Content-Type: " + contentType;

      client.println(statusString);
      client.println(contentTypeString);
      client.println();
      client.println(content);
    }

    void begin() {
      server.begin();
    }

    void handleClient() {
      client = server.available();
      if (client) {
        while (client.connected()) {
          if (client.available()) {
            String request = client.readStringUntil('\r');
            client.flush();
            bool routeFound = false;
            for (const auto& route : routes) {
              if (request.indexOf(route.path) != -1) {
                route.callback();
                routeFound = true;
                break;
              }
            }

            if (!routeFound) {
              handleNotFound();
            }
            break;
          }
        }
        client.stop();
      }
    }
};

WebServer server(80);

long getWiFiSignalStrength() {
  long rssi = WiFi.RSSI();
  return rssi;
}

void printIpAddress() {
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void attachServo(Servo &servo, int pin) {
  if (!servo.attached()) {
    servo.attach(pin);
  }
}

void detachServo(Servo &servo, int pin) {
  if (servo.attached()) {
    servo.detach();
  }
}

void resetServoPositions() {
  if (triggerServo.attached() && primerServo.attached()) {
    triggerServo.write(0);
    primerServo.write(0);
  } else {
    Serial.println("Servo motors not attached!");
  }
}

void handleServoTrigger() {
  attachServo(triggerServo, triggerServoPin);
  attachServo(primerServo, primerServoPin);

  resetServoPositions();
  delay(500);

  primerServo.write(90);
  delay(500);
  triggerServo.write(90);
  delay(1000);
  resetServoPositions();

  detachServo(triggerServo, triggerServoPin);
  detachServo(primerServo, primerServoPin);
}

void handleServo() {
  handleServoTrigger();

  server.send(200, "text/plain", "");
}

void handleRoot() {
  server.send(200, "text/plain", "FF8D-R4 ONLINE");
}

void setup() {
  Serial.begin(9600);

  delay(5000);

  while (!Serial) {
    ; // Wait for Serial port to connect
  }

  Serial.println("Serial started.");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    // TODO: LED Indicator
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("WARN: Firmware out of date");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    
    if (status == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WiFi... Retrying in 10 seconds.");
    }

    delay(10000);
  }

  Serial.println("Connected to WiFi");

  long rssi = getWiFiSignalStrength();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
  
  printIpAddress();

  server.addRoute("/", handleRoot);
  server.addRoute("/servo", handleServo);

  server.begin();
}

void loop() {
  server.handleClient();
}
