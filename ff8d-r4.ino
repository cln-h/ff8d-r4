#include <WiFiS3.h>

#include <vector>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

struct Route {
  String path;
  void (*callback)();
};
class WebServer {
  private:
    WiFiServer server;
    std::vector<Route> routes;

  public:
    WebServer(int port) : server(port) {}
  
    // addRoute() {}
    // send() {}
    // handle 404 in some way
};

long getWiFiSignalStrength() {
  long rssi = WiFi.RSSI();
  return rssi;
}

void setup() {
  Serial.begin(9600);

  while (!Serial) {
    ; // Wait for Serial port to connect
  }

    if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    // TODO: LED Indicator
    while (true);

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("WARN: Firmware out of date");
    }

    while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);

      status = WiFi.begin(ssid, pass);

      delay(10000);
    }

    Serial.println("Connected to WiFi");

    long rssi = getWiFiSignalStrength();
    Serial.print("Signal strength (RSSI): ");
    Serial.println(rssi);

    // server.begin(); // TODO
  }
}

void loop() {
}
