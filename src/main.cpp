#include "Arduino.h"
#include "WiFi.h"
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include "network.h"
#include "telemetry.h"
#include "model.h"
#include "edge_ai.h"
#include "optimization.h"

void setup()
{

    Serial.begin(115200);
    dht.begin();  // initialise sesnor
    configTime(0,0, "pool.ntp.org", "time.nist.gov"); // configure NTP server for time synchronization
    pinMode(BTN_PLUGIN, INPUT_PULLUP); // set the pin as input with pullup resistor
    pinMode(BTN_PLUGOUT, INPUT_PULLUP); // set the pin as input with pullup resistor
    pinMode(RELAY_PIN, OUTPUT); // set the pin as output
    pinMode(LED_GREEN, OUTPUT); // set the pin as output
    pinMode(LED_RED, OUTPUT); // set the pin as output
    pinMode(LED_YELLOW, OUTPUT); // set the pin as output
    connectWiFi();
    
    // Configure MQTT server
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    // The complete telemetry JSON exceeds PubSubClient's default 256-byte
    // packet buffer.
    mqtt.setBufferSize(512);
    connectMQTT();

}

unsigned long now;
unsigned long last_print;

void loop()
{
    mqtt.loop();

    //print vals every 5 sec
    now = millis();
    if((now - last_print) > 5000)
    {
        last_print = now;
        sample_sensor();
        //run edge ai inference
        runEdgeAIInference();
        //run optimization logic decide loadDecision and throttleLevel
        runOptimization();
        //publish telemetry data to the MQTT topic
        publishTelemetry(); 
        

    }
    plug_status();
    updateLeds();

    
}

