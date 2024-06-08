#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define SETTINGS_ADR 10

namespace SaveSetings
{
	/// структура с сохраняемыми в EEPROM параметрами
	struct DeviceSetings
	{
		long checkInterval;	  // проверка через n секунд
		int wateringDuration; //  поливать n секунд
		int limit;			  //  порог датчика
		bool checkSensor;
	};

	DeviceSetings setings;
	bool sensorLoggerIsBegin = false;

	/// функция выводит сообщения в потр при определении "#define DEBUG"
	void dPrint(String message)
	{
#ifdef DEBUG
		Serial.print(message);
#endif
	}

	/// функция выводит сообщения в потр при определении "#define DEBUG"
	void dPrintln(String message)
	{
#ifdef DEBUG
		Serial.println(message);
#endif
	}

	void begin()
	{
		dPrintln("SaveSetings begin");

		EEPROM.begin(500);
		EEPROM.get(SETTINGS_ADR, setings);

		dPrintln("setings:");
		dPrintln("checkInterval: " + String(setings.checkInterval));
		dPrintln("wateringDuration: " + String(setings.wateringDuration));
		dPrintln("limit: " + String(setings.limit));
		dPrintln("checkSensor: " + String(setings.checkSensor));

		sensorLoggerIsBegin = true;
	}

	void saveSetings()
	{
		EEPROM.begin(500);
		EEPROM.put(SETTINGS_ADR, setings);
		EEPROM.commit();
		dPrintln("Save prefs");
	}

}
