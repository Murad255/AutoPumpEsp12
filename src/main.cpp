#include <Arduino.h>
#include <EEPROM.h>

#define DEBUG
#include "WifiConnect.h"

#define PUMP D0
#define LED LED_BUILTIN
#define SENS A0

using SaveSetings::setings;
unsigned long lastCheckTime = 0;
void updateSettings();
void setPump(boolean state);
void pump();

void setup()
{
  Serial.begin(9600);
  pinMode(PUMP, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(SENS, 0);
  digitalWrite(PUMP, 1);

  // begin();

  WifiApSetSettings::сreateConnectPoint(2 * 60);
  pump();
  lastCheckTime = millis() / 1000ul;
  WiFi.forceSleepBegin(); // Modem–Sleep Mode
}

void loop()
{
  uint32_t currentTimeSec = millis() / 1000ul;
  int soilMoisture = analogRead(SENS);
  Serial.println("soilMoisture = " + String(soilMoisture));

  if (currentTimeSec - lastCheckTime >= (uint32_t)(setings.checkInterval))
  {
    Serial.println("Check ");
    if (!setings.checkSensor || soilMoisture >= setings.limit)
    {
      pump();
    }
    lastCheckTime = currentTimeSec;
  }
  // при переполнении
  if (currentTimeSec < lastCheckTime)
    lastCheckTime = currentTimeSec;

  delay(1000);
}

void setPump(boolean state)
{
  digitalWrite(PUMP, !state);
  digitalWrite(LED, !state);
}

void pump()
{
  Serial.println("Soil is dry. Watering " + String(setings.wateringDuration) + " sec.");
  setPump(1);
  delay(setings.wateringDuration * 1000);
  setPump(0);
  Serial.println("Stop pump");
}