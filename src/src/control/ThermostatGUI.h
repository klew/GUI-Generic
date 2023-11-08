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

#if SUPLA_THERMOSTAT
#ifndef _thermostat_gui_h
#define _thermostat_gui_h

#include <Arduino.h>

#include <SuplaDevice.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/control/hvac_base.h>
#include <supla/clock/clock.h>
#include <supla/control/internal_pin_output.h>
#include <supla/events.h>
#include <supla/actions.h>

#include "../../SuplaDeviceGUI.h"
#include "supla/protocol/protocol_layer.h"

#define THERMOSTAT_DEFAULT_HISTERESIS "0.4"
#define THERMOSTAT_NO_TEMP_CHANNEL    0

namespace Supla {
namespace Control {
namespace GUI {

class ThermostatGUI : public Supla::Control::HvacBase, public Supla::Protocol::ProtocolLayer {
 public:
  ThermostatGUI(uint8_t nr, SuplaDeviceClass *sdc);

  virtual void notifyConfigChange(int channelNumber) override;
  virtual void onInit() override {
    HvacBase::onInit();
  };
  virtual bool onLoadConfig() override {
    return true;
  }
  virtual bool verifyConfig() override {
    return true;
  }
  virtual bool isEnabled() override {
    return false;
  }
  virtual void disconnect() override{};
  virtual bool iterate(uint32_t _millis) override {
    return false;
  }
  virtual bool isNetworkRestartRequested() override {
    return false;
  }
  virtual uint32_t getConnectionFailTime() override {
    return false;
  }
  virtual bool isRegisteredAndReady() override {
    return false;
  }

  virtual void sendActionTrigger(uint8_t channelNumber, uint32_t actionId) override{};
  virtual void sendChannelValueChanged(uint8_t channelNumber, char *value, unsigned char offline, uint32_t validityTimeSec) override{};
  virtual void sendExtendedChannelValueChanged(uint8_t channelNumber, TSuplaChannelExtendedValue *value) override{};

  void handleAction(int event, int action) override;

  void setHandleActionBlocked(bool block) {
    blockHandleAction = block;
  }
  bool getHandleActionBlocked() {
    return blockHandleAction;
  }

  void setNrActiveThermostat(uint8_t newValue) {
    nrActiveThermostat = newValue;
  }

  uint8_t getNrActiveThermostat() {
    return nrActiveThermostat;
  }

  void setNrThermostat(uint8_t newValue) {
    nr = newValue;
  }

  uint8_t getNrThermostat() {
    return nr;
  }

 private:
  uint8_t nr;
  uint8_t nrActiveThermostat;
  bool blockHandleAction = false;
  int holdCounter = 0;
};

};  // namespace GUI
};  // namespace Control
};  // namespace Supla

namespace Supla {
namespace GUI {
extern std::array<Supla::Control::GUI::ThermostatGUI *, MAX_THERMOSTAT> thermostat;
}
}  // namespace Supla

#endif
#endif
