#include <WiFiS3.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

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
  }
}

void loop() {
  delay(1000);
  Serial.println("...");
}
