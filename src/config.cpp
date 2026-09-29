#include "config.h"
#include "secrets.h"

// ---------------------------------------------------------------------
// WiFi Details
// ---------------------------------------------------------------------

const char* WIFI_SSID = SECRET_WIFI_SSID;
const char* WIFI_PASS = SECRET_WIFI_PASS;

// ---------------------------------------------------------------------
// ThingsBoard Details
// ---------------------------------------------------------------------

const char* MQTT_SERVER = "mqtt.thingsboard.cloud";
const int MQTT_PORT = 1883;

// ThingsBoard Device Access Token
const char* TB_TOKEN = SECRET_TB_TOKEN;

const char* BAY_ID = "BAY1";