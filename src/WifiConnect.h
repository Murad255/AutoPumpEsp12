#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <GyverPortal.h>

#include "SaveSetings.h"

#define SENS A0
#define PUMP D0

using namespace SaveSetings;

namespace WifiApSetSettings
{
  GyverPortal ui;

  GP_TITLE tit("t1", "Задайте параметры автополива");
  // GP_NUMBER num("num");
  GP_LABEL lab1("lb1", "Время полива");
  GP_SPINNER sp("sp");

  GP_LABEL lab2("lb2", "Периодичность полива");
  GP_TIME tm("tm");

  GP_LABEL lab3("lb3", "Порог срабатывания датчика");
  GP_SLIDER sl("sl", 500, 0, 1023);

  GP_LABEL lab4("lb4", "Использовать датчик");
  GP_SWITCH sw("sw");
  GP_LABEL_BLOCK labb("lbb", "");

  GP_LABEL lab5("lb5", "Ручное управление (ВКЛ полив)");
  GP_SWITCH sw2("sw2");

  GP_BUTTON btn("b1", "Сохранить");

  // ssid и пароль точки доступа
  const char *AP_SSID = "pass:123456789";
  const char *AP_PASS = "123456789";

  String wifiSsid = "";
  String wifiPassword = "";

  int connections = 0;
  volatile boolean closeGui = false;

  void _WiFiStationConnected(WiFiEvent_t event)
  {
    connections += 1;
    Serial.print("connections: ");
    Serial.println(connections);
  }

  void _WiFiStationDisconnected(WiFiEvent_t event)
  {
    connections -= 1;
    Serial.print("disconnections: ");
    Serial.println(connections);
  }

  void _build()
  {
    GP.BUILD_BEGIN(GP_DARK);
    // GP.UPDATE("t1,l1,lb1,led1,led2,led3,num,numf,txt,pas,ar,ch,sw,dat,tm,col,sp,sl");

    GP.TITLE(tit);
    GP.LABEL(lab1);
    GP.SPINNER(sp);

    GP.LABEL(lab2);
    GP.TIME(tm);

    GP.LABEL(lab3);
    GP.SLIDER(sl);
    GP.LABEL_BLOCK(labb);

    GP.LABEL(lab4);
    GP.SWITCH(sw);

    GP.LABEL(lab5);
    GP.SWITCH(sw2);

    GP.BUTTON(btn);

    GP.FORM_BEGIN("/save");
    GP.UPDATE("lbb");

    GP.BUILD_END();
  }

  void _action()
  {
    if (ui.form("/save"))
    {
      Serial.println("save ");

      ui.copy(sw);
      Serial.println(sw.state);

      ui.copy(tm);
      Serial.println(tm.time.encode());

      ui.copy(sp);
      Serial.println(sp.value);

      ui.copy(sl);
      Serial.println(sl.value);
    }
    if (ui.click())
    {
      Serial.println("ui.click ");

      if (ui.clickUp(btn))
        Serial.println("down");
      if (ui.clickDown(btn))
        Serial.println("up");

      if (ui.click(btn))
      {
        Serial.println("btn");
        closeGui = true;
      }

      if (ui.click(sw))
        Serial.println(sw.state);

      if (ui.click(sw2)){
        Serial.println(sw2.state);
        digitalWrite(PUMP, !sw2.state);
      }
      if (ui.click(tm))
        Serial.println(tm.time.encode());

      if (ui.click(sp))
        Serial.println(sp.value);
      if (ui.click(sl))
        Serial.println(sl.value);
    }

    if (ui.update())
    {
        labb.text = "Показание датчика = " + String(analogRead(SENS));
        ui.update(labb);
    }
  }

  /// @brief Функция создаёт точку доступа для ввода настроек
  /// @param timeToConnestSec Время ожидания подключения к точке. При отсутствии подключений загружаются ранее сохранённые настройки
  /// @return Возвращает true в случае ввода новых настроек, иначе false
  bool сreateConnectPoint(long timeToConnestSec = -1)
  {
    begin();
    // btn.text = "button 1";
    // sp.decimals = 0;
    // sl.decimals = 2;
    // sl.step = 1;

    sp.value = setings.wateringDuration;
    tm.time.second = setings.checkInterval % 60;
    tm.time.minute = (setings.checkInterval / 60) % 60;
    tm.time.hour = setings.checkInterval / (60*60);
    sl.value = setings.limit;
    sw.state = setings.checkSensor;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    WiFi.onEvent(_WiFiStationConnected, WiFiEvent_t::WIFI_EVENT_SOFTAPMODE_STACONNECTED);
    WiFi.onEvent(_WiFiStationDisconnected, WiFiEvent_t::WIFI_EVENT_SOFTAPMODE_STADISCONNECTED);

#ifdef DEBUG
    Serial.print("Server begin IP address: ");
    Serial.println(WiFi.softAPIP());
#endif
    ui.attachBuild(_build);
    ui.attach(_action);
    ui.start();

    unsigned long startTime = millis();
    unsigned long startTime2 = millis();

    while (!closeGui)
    {

      ui.tick();
      unsigned long currentTime = millis();
      if (currentTime - startTime >= (timeToConnestSec * 1000) && connections == 0)
        break;
    }
    WiFi.mode(WIFI_OFF);

    if (closeGui)
    {
      setings.wateringDuration = (int)sp.value;
      setings.checkInterval = tm.time.second + tm.time.minute * 60 + tm.time.hour * 60 * 60;
      setings.limit = sl.value;
      setings.checkSensor = sw.state;
      saveSetings();

      dPrintln("duration = " + String(setings.wateringDuration));
      dPrintln("period = " + String(setings.checkInterval));
      dPrintln("limit = " + String(setings.limit));
      dPrintln("useSensor = " + String(setings.checkSensor));

      return true;
    }
    else
    {
      dPrintln("Use old setings");
      return false;
    }
  }

}