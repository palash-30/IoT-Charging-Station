#include <Arduino.h>
#include <DHT.h>
#include "State.h"
#include "Peripherals.h"
#include "config.h"


DHT   dht(DHT_PIN , DHT_TYPE);

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void sample_sensor(void)
{
   int raw_current = analogRead(CURRENT_PIN); // 0 to 4095
   int raw_voltage = analogRead(VOLTAGE_PIN); // 0 to 4095
   
   voltage = mapfloat(raw_voltage, 0, 4095, 0, 250); // 0 to 250V

   if(bayStatus == "CHARGING")
   {
       current = mapfloat(raw_current, 0, 4095, 0, 32); // 0 to 32A
   }
   else
   {
       current = 0.0f;
   }

   //read current and 5 values array

   //calculate power 
   power = voltage * current;

   // Keep the most recent valid DHT measurements if a read temporarily fails.
   float t = dht.readTemperature();
   if (!(isnan(t))) temperature = t;
   float h = dht.readHumidity();
   if (!(isnan(h))) humidity = h;
}

float recentAvgCurrent(void)
{
    
    float sum = 0;
    for (int i = 0; i < 5; i++)
    {
        sum = sum + current;
    }
    return sum / 5;
}

bool plugin_flag_once = 1;
bool plugout_flag_once = 1;

void plug_status(void)
{ 
   bool pluginReading = digitalRead(BTN_PLUGIN);
   // detect the sw is pressed
   if (pluginReading == LOW && plugin_flag_once ) // switch is pressed
   {
      //session time start
      sessionStartMs = millis();
      // plug in switch is pressed
      plugin_flag_once = 0;
      // change bay_status FREE to charging
      if (bayStatus == "FREE")
      {
          bayStatus = "CHARGING";
          Serial.println("Bay1 Status is CHARGING");
          digitalWrite(RELAY_PIN, HIGH); // turn on the relay to start charging
      }
      //update leds
   }
   
   if (pluginReading == HIGH) // switch is released
   {
      plugin_flag_once = 1;
   }

   bool plugoutReading = digitalRead(BTN_PLUGOUT);
   // detect the sw is pressed
   if (plugoutReading == LOW && plugout_flag_once ) // switch is pressed
   {
      // plug out switch is pressed
      plugout_flag_once  = 0;
      // change bay_status charging to FREE
      if (bayStatus == "CHARGING")
      {
          bayStatus = "FREE";
          Serial.println("Bay1 Status is FREE");
      }
   }
   if (plugoutReading == HIGH) // switch is released
   {
      plugout_flag_once = 1;
   }
   // plug out switch is pressed
   // change bay_status  charging to FREE
   //update leds
}

void update_led_status(void)
{
    if (bayStatus == "FREE")
    {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
    }
    else 
    {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, HIGH);
    }
}
