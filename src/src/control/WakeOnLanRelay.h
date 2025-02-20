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

#ifndef _Wake_On_Lan_Relay_h
#define _Wake_On_Lan_Relay_h

#include <Arduino.h>
#include <WakeOnLan.h>
#include <supla/control/virtual_relay.h>

#define MAC_ADDRESS_SIZE 18

namespace Supla {
namespace Control {

class WakeOnLanRelay : public VirtualRelay {
 public:
  WakeOnLanRelay(const char *MACAddress = nullptr);

  void onInit();
  void turnOn(_supla_int_t duration = 0);
  void wakePC();

 private:
  WiFiUDP UDP;
  WakeOnLan WOL;
  char MACAddress[MAC_ADDRESS_SIZE];
};

};  // namespace Control
};  // namespace Supla
#endif