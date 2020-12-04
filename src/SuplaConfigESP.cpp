/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <ESP8266WiFi.h>

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaDeviceGUI.h"
#include "GUIGenericCommon.h"

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = NORMAL_MODE;

  if (ConfigManager->isDeviceConfigured()) {
    ConfigManager->setGUIDandAUTHKEY();
    if (String(ConfigManager->get(KEY_LOGIN)->getValue()) == 0) {
      ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    }
    if (String(ConfigManager->get(KEY_LOGIN_PASS)->getValue()) == 0) {
      ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    }
    if (String(ConfigManager->get(KEY_HOST_NAME)->getValue()) == 0) {
      ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    }
    if (String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue()) == 0) {
      ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    }
    if (String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue()) == 0) {
      ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    }
    ConfigManager->save();

    configModeInit();
  }

  SuplaDevice.setStatusFuncImpl(&status_func);
}

void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton, bool _ledHighIsOn) {
  pinNumberConfig = _pinNumberConfig;
  pinLedConfig = _pinLedConfig;
  modeConfigButton = _modeConfigButton;
  ledHighIsOn = _ledHighIsOn;

  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO) {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, pinOffValue());
  }

  if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) != OFF_GPIO) {
    Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, true, true);
    buttonConfig->setMulticlickTime(1000);
    buttonConfig->addAction(Supla::TURN_ON, *ConfigESP, Supla::ON_CLICK_1);

    if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
      buttonConfig->addAction(CONFIG_MODE_10_ON_PRESSES, *ConfigESP, Supla::ON_CLICK_10);
    }
    if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
      buttonConfig->setHoldTime(5000);
      buttonConfig->addAction(CONFIG_MODE_5SEK_HOLD, *ConfigESP, Supla::ON_HOLD);
    }
  }
}

void SuplaConfigESP::runAction(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (event == Supla::ON_CLICK_10) {
      configModeInit();
    }
  }
  if (action == CONFIG_MODE_5SEK_HOLD) {
    if (event == Supla::ON_HOLD) {
      configModeInit();
    }
  }

  if (configModeESP == CONFIG_MODE) {
    if (event ==  Supla::ON_CLICK_1) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::rebootESP() {
  delay(1000);
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1) wdt_reset();
}

void SuplaConfigESP::configModeInit() {
  configModeESP = CONFIG_MODE;
  ledBlinking(100);

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
}

void SuplaConfigESP::iterateAlways() {
  if (configModeESP == CONFIG_MODE) {
    String CONFIG_WIFI_NAME = "SUPLA-ESP8266-" + getMacAddress(false);
    WiFi.softAP(CONFIG_WIFI_NAME, "");
  }
}

const char *SuplaConfigESP::getLastStatusSupla() {
  return supla_status.msg;
}

void SuplaConfigESP::ledBlinking(int time) {
  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, ledBlinking_func, NULL);
  os_timer_arm(&led_timer, time, true);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(pinLedConfig, pinOffValue());
}

int SuplaConfigESP::getPinLedConfig() {
  return pinLedConfig;
}

String SuplaConfigESP::getMacAddress(bool formating) {
  byte mac[6];
  WiFi.macAddress(mac);
  char baseMacChr[18] = {0};

  if (formating)
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  else
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return String(baseMacChr);
}

void ledBlinking_func(void *timer_arg) {
  int val = digitalRead(ConfigESP->getPinLedConfig());
  digitalWrite(ConfigESP->getPinLedConfig(), val == HIGH ? 0 : 1);
}

void status_func(int status, const char *msg) {
#ifndef UI_LANGUAGE
  switch (status) {
    case 2:
      ConfigESP->supla_status.msg = S_ALREADY_INITIATED;
      break;
    case 3:
      ConfigESP->supla_status.msg = S_NOT_ASSIGNED_CB;
      break;
    case 4:
      ConfigESP->supla_status.msg = S_INVALID_GUID_OR_DEVICE_REGISTRATION_INACTIVE;
      break;
    case 5:
      ConfigESP->supla_status.msg = S_UNKNOWN_SEVER_ADDRESS;
      break;
    case 6:
      ConfigESP->supla_status.msg = S_UNKNOWN_ID;
      break;
    case 7:
      ConfigESP->supla_status.msg = S_INITIATED;
      break;
    case 8:
      ConfigESP->supla_status.msg = S_CHANNEL_LIMIT_EXCEEDED;
      break;
    case 9:
      ConfigESP->supla_status.msg = S_DISCONNECTED;
      break;
    case 10:
      ConfigESP->supla_status.msg = S_REGISTRATION_IS_PENDING;
      break;
    case 11:
      ConfigESP->supla_status.msg = S_VARIABLE_ERROR;
      break;
    case 12:
      ConfigESP->supla_status.msg = S_PROTOCOL_VERSION_ERROR;
      break;
    case 13:
      ConfigESP->supla_status.msg = S_BAD_CREDENTIALS;
      break;
    case 14:
      ConfigESP->supla_status.msg = S_TEMPORARILY_UNAVAILABLE;
      break;
    case 15:
      ConfigESP->supla_status.msg = S_LOCATION_CONFLICT;
      break;
    case 16:
      ConfigESP->supla_status.msg = S_CHANNEL_CONFLICT;
      break;
    case 17:
      ConfigESP->supla_status.msg = S_REGISTERED_AND_READY;
      break;
    case 18:
      ConfigESP->supla_status.msg = S_DEVICE_IS_DISCONNECTED;
      break;
    case 19:
      ConfigESP->supla_status.msg = S_LOCATION_IS_DISABLED;
      break;
    case 20:
      ConfigESP->supla_status.msg = S_DEVICE_LIMIT_EXCEEDED;
  }
#else
  ConfigESP->supla_status.msg = msg;
#endif

  static int lock;
  if (status == 17 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinkingStop();
    lock = 0;
  }
  else if (status != 17 && lock == 0 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinking(500);
    lock = 1;
  }

  if (ConfigESP->supla_status.old_msg != ConfigESP->supla_status.msg) {
    ConfigESP->supla_status.old_msg = ConfigESP->supla_status.msg;
    ConfigESP->supla_status.status = status;
    //    Serial.println(ConfigESP->supla_status.msg);
  }
}

int SuplaConfigESP::getGpio(int nr, int function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;
    if (function == FUNCTION_CFG_BUTTON) {
      if (checkBusyCfg(gpio)) {
        return gpio;
      }
    }
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key)->getElement(NR).toInt() == nr) {
        return gpio;
      }
    }
  }
  return OFF_GPIO;
}

int SuplaConfigESP::getLevel(int nr, int function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key)->getElement(NR).toInt() == nr) {
        uint8_t level = ConfigManager->get(key)->getElement(LEVEL).toInt();
        return level;
      }
    }
  }
  return OFF_GPIO;
}

int SuplaConfigESP::getMemory(int nr, int function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key)->getElement(NR).toInt() == nr) {
        uint8_t level = ConfigManager->get(key)->getElement(MEMORY).toInt();
        return level;
      }
    }
  }
  return OFF_GPIO;
}

int SuplaConfigESP::getAction(int nr, int function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key)->getElement(NR).toInt() == nr) {
        uint8_t action = ConfigManager->get(key)->getElement(ACTION).toInt();
        return action;
      }
    }
  }
  return OFF_GPIO;
}

bool SuplaConfigESP::checkBusyCfg(int gpio) {
  uint8_t key = KEY_GPIO + gpio;
  if (ConfigManager->get(key)->getElement(FUNCTION_CFG_LED).toInt() == 1) {
    return true;
  }
  return false;
}

int SuplaConfigESP::checkBusyGpio(int gpio, int function) {
  if (gpio == 6 || gpio == 7 || gpio == 8 || gpio == 11) {
    return true;
  }
  else {
    uint8_t key = KEY_GPIO + gpio;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_BUTTON) {
      if (function == FUNCTION_CFG_BUTTON) {
        return false;
      }
    }
    if (checkBusyCfg(gpio)) {
      if (function != FUNCTION_BUTTON) {
        return true;
      }
    }
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_OFF) {
      if (ConfigManager->get(key)->getElement(FUNCTION).toInt() != function) {
        return true;
      }
    }
    return false;
  }
}

void SuplaConfigESP::setGpio(uint8_t gpio, uint8_t nr, uint8_t function, uint8_t level, uint8_t memory) {
  uint8_t key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 1);
    return;
  }

  ConfigManager->setElement(key, NR, nr);
  ConfigManager->setElement(key, FUNCTION, function);
  ConfigManager->setElement(key, LEVEL, level);
  ConfigManager->setElement(key, MEMORY, memory);
  ConfigManager->setElement(key, ACTION, Supla::TOGGLE);

  // ConfigManager->setElement(key.c_str(), MEMORY, memory);
  // ConfigManager->setElement(key.c_str(), CFG, cfg);
}

void SuplaConfigESP::clearGpio(uint8_t gpio, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 0);
    return;
  }
  ConfigManager->setElement(key, NR, 0);
  ConfigManager->setElement(key, FUNCTION, FUNCTION_OFF);
  ConfigManager->setElement(key, LEVEL, 0);
  ConfigManager->setElement(key, MEMORY, 0);
  ConfigManager->setElement(key, ACTION, Supla::TOGGLE);
}

uint8_t SuplaConfigESP::countFreeGpio(uint8_t exception) {
  uint8_t count = 0;
  for (uint8_t gpio = 0; gpio < OFF_GPIO; gpio++) {
    if (gpio != 6 && gpio != 7 && gpio != 8 && gpio != 11) {
      uint8_t key = KEY_GPIO + gpio;
      if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
          ConfigManager->get(key)->getElement(FUNCTION).toInt() == exception) {
        count++;
      }
    }
  }
  return count;
}

void SuplaConfigESP::factoryReset() {
  delay(1000);
  pinMode(0, INPUT);
  if (!digitalRead(0)) {
    Serial.println(F("FACTORY RESET!!!"));

    ConfigManager->set(KEY_WIFI_SSID, "");
    ConfigManager->set(KEY_WIFI_PASS, "");
    ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    ConfigManager->set(KEY_MAX_RELAY, "1");
    ConfigManager->set(KEY_MAX_BUTTON, "1");
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
    ConfigManager->set(KEY_MAX_DHT22, "1");
    ConfigManager->set(KEY_MAX_DHT11, "1");
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, "1");
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, "0");
    ConfigManager->set(KEY_ALTITUDE_BME280, "0");
    ConfigManager->set(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, "0");
    ConfigManager->set(KEY_MAX_IMPULSE_COUNTER, "0");
    ConfigManager->set(KEY_ACTIVE_SENSOR, "0,0,0,0,0");
    ConfigManager->set(KEY_BOARD, "0");
    ConfigManager->set(KEY_CFG_MODE, "0");

    uint8_t nr, key;
    for (nr = 0; nr <= 17; nr++) {
      key = KEY_GPIO + nr;
      ConfigManager->set(key, "0,0,0,0,0");
    }

    for (nr = 0; nr <= MAX_DS18B20; nr++) {
      key = KEY_DS + nr;
      ConfigManager->set(key, "");
      key = KEY_DS_NAME + nr;
      ConfigManager->set(key, "");
    }

    ConfigManager->save();

    rebootESP();
  }
}
