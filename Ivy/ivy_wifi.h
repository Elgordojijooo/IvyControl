#ifndef IVY_WIFI_H
#define IVY_WIFI_H

#include <WiFi.h>
#include <WebServer.h>
#include "ivy_nuts_and_bolts.h" // assuming it includes timeinfo and sensors struct
#include "ivy_relay.h"
#include "page.h"

#define LED_PIN 2 // Indicator LED (optional)

WebServer server(80);
String ivy_wifi_last_ip = "---.---.---.---";

// ======= Set Static IP =======
IPAddress local_IP(192, 168, 1, 42);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// ======= Your Wi-Fi Credentials =======
const char* ssid = "TP-Link_BC20";
const char* password = "18753486";

// ======= Function: Connect to Wi-Fi =======
void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi: "); Serial.println(ssid);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Connected!");
    Serial.print("Local IP: "); Serial.println(WiFi.localIP());
    ivy_wifi_last_ip = WiFi.localIP().toString();
  } else {
    Serial.println("\nWi-Fi Connection Failed!");
  }
}

// Server internal variables used for the UI
int humidityHistory24h[24];
int humidityHistory1h[60];
int humidityHistory5m[60];
int tempHistory24h[24];
int tempHistory1h[60];
int tempHistory5m[60];

String printHistory(int* data, int size) {
  String r = "";
  for (int i = 0; i < size; i++) {
    r += String(data[i]);
    if (i < size - 1) r += ", ";
  }
  return r;
}

String printOutlets() {
  String r = "";
  for (int i = 0; i < N_OUTLETS; i++) {
    r += "{";
    r += "\"label\":\""; r += outlet[i].label; r += "\",";
    r += "\"state\":"; r += outlet[i].current_state; r += ",";
    r += "\"mode\":"; r += outlet[i].mode;
    r += "}";
    if (i < N_OUTLETS - 1) r += ", ";
  }
  return r;
}

// ======= Function: Serve Monitoring Page =======
void setupWebServer() {
  // Root
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", page);
  });

  // Sensors
  server.on("/status", HTTP_GET, []() {
    String json = "{";
    // Clock
    json += "\"clock\": \"" + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + "\",";


    // Temperature Views
    json +=
    "\"temp\": {\n"
      "\"current\": \"" + String((int)(sensors.temperature)) + "\",\n"
      "\"log\": {\n"
      "\"last_24_hours\": [" + printHistory(tempHistory24h, 24) + "],\n"
      "\"last_hour\": [" + printHistory(tempHistory1h, 60) + "],\n"
      "\"last_5_minutes\": [" + printHistory(tempHistory5m, 60) + "]\n"
      "}\n"
    "},";

    // Humidity Views
    json +=
    " \"humidity\": { "
      " \"current\": \" " + String((int)(sensors.humidity)) + " \","
      " \"log\": { "
        " \"last_24_hours\":[ "  + printHistory(humidityHistory24h, 24) + "],"
        " \"last_hour\":[ "      + printHistory(humidityHistory1h,  60) + "],"
        " \"last_5_minutes\":[ " + printHistory(humidityHistory5m,  60) + "]"
      "}"
    "},";


    // Humidity Views
    json +=
      " \"outlets\": [ "
        + printOutlets() +
      "]";

    json += "}";
    Serial.println(json);
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Web server started");
}












// ======= WiFi Begin Wrapper =======
void ivy_wifi_begin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  connectToWiFi();
  setupWebServer();
}




// ======= WiFi Loop Wrapper =======
void ivy_wifi_loop() {
  server.handleClient();
}

#endif
