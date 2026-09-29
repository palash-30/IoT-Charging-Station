#include <ArduinoJson.h>
#include "telemetry.h"
#include "network.h"
#include "state.h"
#include "config.h"

// ---------------------------------------------------------------------
// FR-7 telemetry publish. Adds `overloadActive` (additive, beyond SRS
// 8.3's baseline schema) so the ThingsBoard Overcurrent alarm rule can
// be a one-line filter. Also adds `manualOverrideActive` so the
// dashboard can show when a bay is under operator control instead of
// automatic optimization.
// ---------------------------------------------------------------------
void publishTelemetry() 
{
  // check if MQTT is connected before publishing telemetry
  if (!mqtt.connected()) return;
    
  // store telemetry data in a JSON document and publish it to the MQTT topic
  StaticJsonDocument<512> doc;
  doc["bayId"] = BAY_ID;
  doc["voltage"] = round(voltage * 10) / 10.0;
  doc["current"] = round(current * 10) / 10.0;
  doc["power"] = round(power * 10) / 10.0;
  doc["temperature"] = round(temperature * 10) / 10.0;
  doc["humidity"] = round(humidity * 10) / 10.0;
  doc["bayStatus"] = bayStatus;
  doc["predictedArrivalProb"] = round(predictedArrivalProb * 100) / 100.0;
  doc["predictedDurationMin"] = predictedDurationMin;
  doc["lastHourOfDay"] = lastHourOfDay;
  //add load decision and throttle level to the telemetry data
  doc["loadDecision"] = loadDecision;
  doc["throttleLevel"] = throttleLevel;
  doc["overloadActive"] = overloadActive;

  char buffer[512];
  serializeJson(doc, buffer);


  //push the telemetry data to the MQTT topic and print it to the serial monitor
  bool published = mqtt.publish("v1/devices/me/telemetry", buffer);
  Serial.print(published ? "[MQTT >>] " : "[MQTT FAILED] ");
  Serial.println(buffer);
}
