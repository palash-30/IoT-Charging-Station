#include <WiFi.h>
#include "network.h"
#include "config.h"
#include "rpc.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---------------------------------------------------------------------
// MQTT callback
// ---------------------------------------------------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("[MQTT <<] Topic: ");
  Serial.println(topic);

  // Convert payload to null-terminated String
  char message[512];

  if (length >= sizeof(message)) {
    length = sizeof(message) - 1;
  }

  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("[MQTT <<] Payload: ");
  Serial.println(message);

  String topicStr = String(topic);

  // ---------------------------------------------------------------
  // ThingsBoard RPC request
  // Topic:
  // v1/devices/me/rpc/request/<requestId>
  // ---------------------------------------------------------------
  String rpcPrefix = "v1/devices/me/rpc/request/";

  if (topicStr.startsWith(rpcPrefix)) {

    String requestId = topicStr.substring(rpcPrefix.length());

    Serial.print("[RPC] Request ID: ");
    Serial.println(requestId);

    Serial.print("[RPC] Payload: ");
    Serial.println(message);

    handleRpc(requestId, message);

    return;
  }

  // Other MQTT messages can be handled here if required.
}

// ---------------------------------------------------------------------
// WiFi
// ---------------------------------------------------------------------
void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 15000) {

    delay(300);
    Serial.print(".");
  }

  Serial.println(
    WiFi.status() == WL_CONNECTED
      ? " connected."
      : " FAILED (will retry)."
  );
}

// ---------------------------------------------------------------------
// MQTT
// ---------------------------------------------------------------------
void connectMQTT() {

  if (WiFi.status() != WL_CONNECTED)
    return;

  Serial.print("Connecting to ThingsBoard MQTT...");

  if (mqtt.connect(BAY_ID, TB_TOKEN, NULL)) {

    Serial.println(" connected.");

    // Set MQTT callback
    mqtt.setCallback(mqttCallback);

    // Subscribe to ThingsBoard RPC requests
    if (mqtt.subscribe("v1/devices/me/rpc/request/+")) {

      Serial.println("[MQTT] RPC subscription successful.");

    } else {

      Serial.println("[MQTT] RPC subscription FAILED!");
    }

  } else {

    Serial.print(" failed, rc=");
    Serial.println(mqtt.state());

    delay(1000);
  }
}

// ---------------------------------------------------------------------
// Maintain network
// ---------------------------------------------------------------------
void maintainNetwork() {

  static unsigned long lastRetryMs = 0;

  if (WiFi.status() == WL_CONNECTED && mqtt.connected()) {

    mqtt.loop();

    return;
  }

  unsigned long now = millis();

  if (now - lastRetryMs < 5000)
    return;

  lastRetryMs = now;

  if (WiFi.status() != WL_CONNECTED)
    connectWiFi();

  if (WiFi.status() == WL_CONNECTED && !mqtt.connected())
    connectMQTT();

  if (mqtt.connected())
    mqtt.loop();
}