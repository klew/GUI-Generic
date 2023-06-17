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

#ifndef _SSD1306_OLED_H
#define _SSD1306_OLED_H

#include <supla/action_handler.h>
#include <supla/element.h>
#include <supla/actions.h>
#include <supla/control/button.h>
#include <SuplaDevice.h>
#include <supla/clock/clock.h>

#include <Wire.h>
#include <SH1106Wire.h>  //OLED 1.3"
#include <OLEDDisplayUi.h>

#include "../../../SuplaDeviceGUI.h"

#define supla_logo_width  48
#define supla_logo_height 48
const uint8_t supla_logo_bits[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0x00,
    0x00, 0xFC, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x7C, 0x00, 0x00, 0x00, 0x80, 0x03, 0xE0, 0x00, 0x00, 0x00,
    0xC0, 0x01, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0x01, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x00, 0x80, 0x03, 0x00, 0x00, 0xE0, 0x00, 0x80, 0x03, 0x00, 0x00,
    0xE0, 0x00, 0x80, 0x07, 0x00, 0x00, 0xE0, 0x00, 0x80, 0x07, 0x00, 0x00, 0xC0, 0x01, 0xC0, 0x0F, 0x00, 0x00, 0xC0, 0x01, 0xC0, 0x3F, 0x00, 0x00,
    0x80, 0x03, 0xE0, 0x78, 0x00, 0x00, 0x00, 0x1F, 0x7C, 0xE0, 0x01, 0x00, 0x00, 0xFF, 0x7F, 0xC0, 0x03, 0x00, 0x00, 0xFC, 0x1F, 0x00, 0x0F, 0x00,
    0x00, 0xE0, 0x0F, 0x00, 0x9E, 0x07, 0x00, 0x00, 0x0F, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x1E, 0x00, 0x70, 0x18, 0x00, 0x00, 0x1C, 0x00, 0x30, 0x30,
    0x00, 0x00, 0x3C, 0x00, 0x30, 0x30, 0x00, 0x00, 0x38, 0x00, 0x30, 0x30, 0x00, 0x00, 0x78, 0x00, 0x30, 0x30, 0x00, 0x00, 0x70, 0x00, 0x70, 0x18,
    0x00, 0x00, 0x70, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0xE0, 0x00, 0xBC, 0x07, 0x00, 0x00, 0xC0, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x80, 0x01, 0x07, 0x00,
    0x00, 0x00, 0x80, 0x81, 0x03, 0x00, 0x00, 0x00, 0x80, 0xC3, 0x01, 0x00, 0x00, 0x00, 0x00, 0xE3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x31, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x30, 0x00, 0x00,
    0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

enum customActions { OLED_TURN_ON, OLED_NEXT_FRAME };

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state);
void drawFrameStart(OLEDDisplay* display);
void drawFrameConfigMode(OLEDDisplay* display);
void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawFrame2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);

Supla::Channel* getChanelByChannelNumber(int channelNumber);

namespace Supla {
namespace Display {
class SSD1306 : public Supla::ActionHandler, public Supla::Element {
 public:
  SSD1306(
      uint8_t _address, int _sda = -1, int _scl = -1, OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64, HW_I2C _i2cBus = I2C_ONE, int _frequency = 700000);
  void addButton(Supla::Control::Button* button);

 private:
  void onInit();
  void iterateAlways();
  void handleAction(int event, int action);

  OLEDDisplay* display;
  OLEDDisplayUi* ui;

  FrameCallback frames[2];
  OverlayCallback overlays[1];

  unsigned long timeLastChangeOled = millis();
  bool oledON = true;
  bool firstStart = true;
};
}  // namespace Display
}  // namespace Supla
#endif