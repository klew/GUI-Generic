/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <arduino_mock.h>
#include <supla/control/button.h>

using ::testing::Return;

class ActionHandlerMock : public Supla::ActionHandler {
 public:
  MOCK_METHOD(void, handleAction, (int, int), (override));
};

class TimeInterfaceStub2 : public TimeInterface {
  public:
    TimeInterfaceStub2() : value(0) {}

    virtual unsigned long millis() override {
      return value;
    }

    void advance(int advanceMs) {
      value += advanceMs;
    }

    unsigned long value;
};

TEST(ButtonTests, OnPressAndOnRelease) {
  TimeInterfaceStub2 time;
  DigitalInterfaceMock ioMock;
  ActionHandlerMock mock1;

  EXPECT_CALL(ioMock, pinMode(5, INPUT));
  EXPECT_CALL(ioMock, digitalRead(5))
    .WillOnce(Return(0)) // #1 
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 second read, should be ignored
    .WillOnce(Return(1)) // #4 third read, should trigger on_press
    .WillOnce(Return(1)) // #5 time 90 
    .WillOnce(Return(0)) // #6 time 100 
    .WillOnce(Return(0)) // #7 time 110 
    .WillOnce(Return(0)) // #8 time 150 
    ;

  EXPECT_CALL(mock1, handleAction(Supla::ON_PRESS, 1)).Times(1);
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, 2)).Times(2);
  EXPECT_CALL(mock1, handleAction(Supla::ON_RELEASE, 3)).Times(1);

  Supla::Control::Button button(5, false, false);
  button.onInit(); // #1
  button.addAction(1, mock1, Supla::ON_PRESS);
  button.addAction(2, mock1, Supla::ON_CHANGE);
  button.addAction(3, mock1, Supla::ON_RELEASE);

  time.advance(1000);
  button.onTimer(); // #2
  time.advance(10);
  button.onTimer(); // #3 within filtering time - nothing should happen
  time.advance(20);
  button.onTimer(); // #4 ON_PRESS
  time.advance(60);
  button.onTimer(); // #5 
  time.advance(10);
  button.onTimer(); // #6 new state candidate
  time.advance(10);
  button.onTimer(); // #7
  time.advance(20);
  button.onTimer(); // #8 on release
  time.advance(30);
  button.onTimer(); // #
  time.advance(10);
  button.onTimer();
  time.advance(10);
  button.onTimer();
}

TEST(ButtonTests, OnHold) {
  TimeInterfaceStub2 time;
  DigitalInterfaceMock ioMock;
  ActionHandlerMock mock1;

  EXPECT_CALL(ioMock, pinMode(5, INPUT));
  EXPECT_CALL(ioMock, digitalRead(5))
    .WillOnce(Return(0)) // #1 onInit
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 
    .WillOnce(Return(1)) // #4 
    .WillOnce(Return(1)) // #5
    .WillOnce(Return(1)) // #6
    .WillOnce(Return(0)) // #7
    .WillOnce(Return(0)) // #8
    // second round
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 
    .WillOnce(Return(1)) // #4 
    .WillOnce(Return(1)) // #5
    .WillOnce(Return(1)) // #6
    .WillOnce(Return(0)) // #7
    .WillOnce(Return(0)) // #8
    ;                        


  EXPECT_CALL(mock1, handleAction(Supla::ON_PRESS, 1)).Times(2);
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, 2)).Times(4);
  EXPECT_CALL(mock1, handleAction(Supla::ON_RELEASE, 3)).Times(2);
  EXPECT_CALL(mock1, handleAction(Supla::ON_HOLD, 4)).Times(2);

  Supla::Control::Button button(5, false, false);
  button.setHoldTime(500);
  button.onInit();
  button.addAction(1, mock1, Supla::ON_PRESS);
  button.addAction(2, mock1, Supla::ON_CHANGE);
  button.addAction(3, mock1, Supla::ON_RELEASE);
  button.addAction(4, mock1, Supla::ON_HOLD);

  for (int i = 0; i < 2; i++) {
    time.advance(1000);
    button.onTimer(); // #2
    time.advance(30);
    button.onTimer(); // #3 ON_PRESS
    time.advance(60);
    button.onTimer(); // #4
    time.advance(450);
    button.onTimer(); // #5 ON_HOLD
    time.advance(10000);
    button.onTimer(); // #6 
    time.advance(10); 
    button.onTimer(); // #7 new state candidate
    time.advance(30);
    button.onTimer(); // #8 ON_RELEASE
  }
}

TEST(ButtonTests, OnHoldRepeated) {
  TimeInterfaceStub2 time;
  DigitalInterfaceMock ioMock;
  ActionHandlerMock mock1;

  EXPECT_CALL(ioMock, pinMode(5, INPUT));
  EXPECT_CALL(ioMock, digitalRead(5))
    .WillOnce(Return(0)) // #1 onInit
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 
    .WillOnce(Return(1)) // #4 
    .WillOnce(Return(1)) // #5
    .WillOnce(Return(1)) // #6
    .WillOnce(Return(1)) // #7
    .WillOnce(Return(1)) // #8
    .WillOnce(Return(0)) // #9
    .WillOnce(Return(0)) // #10
    ;                        


  EXPECT_CALL(mock1, handleAction(Supla::ON_PRESS, 1)).Times(1);
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, 2)).Times(2);
  EXPECT_CALL(mock1, handleAction(Supla::ON_RELEASE, 3)).Times(1);
  EXPECT_CALL(mock1, handleAction(Supla::ON_HOLD, 4)).Times(3);

  Supla::Control::Button button(5, false, false);
  button.setHoldTime(500);
  button.repeatOnHoldEvery(100);
  button.onInit();
  button.addAction(1, mock1, Supla::ON_PRESS);
  button.addAction(2, mock1, Supla::ON_CHANGE);
  button.addAction(3, mock1, Supla::ON_RELEASE);
  button.addAction(4, mock1, Supla::ON_HOLD);

  time.advance(1000);
  button.onTimer(); // #2
  time.advance(30);
  button.onTimer(); // #3 ON_PRESS
  time.advance(60);
  button.onTimer(); // #4
  time.advance(450);
  button.onTimer(); // #5 ON_HOLD
  time.advance(110);
  button.onTimer(); // #6 ON_HOLD
  time.advance(10); 
  button.onTimer(); // #7 
  time.advance(100);
  button.onTimer(); // #8 ON_HOLD
  time.advance(10);
  button.onTimer(); // #9 new state candidate
  time.advance(30);
  button.onTimer(); // #10 ON_RELEASE
}

TEST(ButtonTests, Multiclick) {
  TimeInterfaceStub2 time;
  DigitalInterfaceMock ioMock;
  ActionHandlerMock mock1;

  EXPECT_CALL(ioMock, pinMode(5, INPUT));
  EXPECT_CALL(ioMock, digitalRead(5))
    .WillOnce(Return(0)) // #1 onInit
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 click 1
    .WillOnce(Return(0)) // #4 
    .WillOnce(Return(0)) // #5 release
    .WillOnce(Return(1)) // #6
    .WillOnce(Return(1)) // #7 click 2
    .WillOnce(Return(0)) // #8
    .WillOnce(Return(0)) // #9 release
    .WillOnce(Return(0)) // #10 some time -> ON_CLICK_2
    ;                        


  EXPECT_CALL(mock1, handleAction(Supla::ON_CLICK_2, 1)).Times(1);
  EXPECT_CALL(mock1, handleAction(Supla::ON_HOLD, 4)).Times(0);

  Supla::Control::Button button(5, false, false);
  button.setHoldTime(700);
  button.setMulticlickTime(300);
  button.onInit();
  button.addAction(4, mock1, Supla::ON_HOLD);
  button.addAction(1, mock1, Supla::ON_CLICK_1);
  button.addAction(1, mock1, Supla::ON_CLICK_2);
  button.addAction(1, mock1, Supla::ON_CLICK_3);
  button.addAction(1, mock1, Supla::ON_CLICK_4);
  button.addAction(1, mock1, Supla::ON_CLICK_5);
  button.addAction(1, mock1, Supla::ON_CLICK_6);
  button.addAction(1, mock1, Supla::ON_CLICK_7);

  time.advance(1000);
  button.onTimer(); // #2
  time.advance(30); // filtering
  button.onTimer(); // #3 click 1
  time.advance(60); // debounce
  button.onTimer(); // #4 
  time.advance(30); // filtering
  button.onTimer(); // #5 release
  time.advance(60); // debounce
  button.onTimer(); // #6 
  time.advance(30); // filtering
  button.onTimer(); // #7 click 2
  time.advance(60); // debounce
  button.onTimer(); // #8 
  time.advance(30); // filtering
  button.onTimer(); // #9 release
  time.advance(500);
  button.onTimer(); // #10 ON_CLICK_2
}

TEST(ButtonTests, OnHoldCancelsMulticlick) {
  TimeInterfaceStub2 time;
  DigitalInterfaceMock ioMock;
  ActionHandlerMock mock1;

  EXPECT_CALL(ioMock, pinMode(5, INPUT));
  EXPECT_CALL(ioMock, digitalRead(5))
    .WillOnce(Return(0)) // #1 onInit
    .WillOnce(Return(1)) // #2 time 0 - first read
    .WillOnce(Return(1)) // #3 click 1
    .WillOnce(Return(1)) // #4 ON_HOLD
    .WillOnce(Return(0)) // #5 
    .WillOnce(Return(0)) // #6 release
    .WillOnce(Return(1)) // #7
    .WillOnce(Return(1)) // #8 click 2
    .WillOnce(Return(0)) // #9
    .WillOnce(Return(0)) // #10 release
    .WillOnce(Return(0)) // #11 some time -> no ON_CLICK_2
    ;                        


  EXPECT_CALL(mock1, handleAction(Supla::ON_CLICK_2, 1)).Times(0);
  EXPECT_CALL(mock1, handleAction(Supla::ON_HOLD, 4)).Times(1);

  Supla::Control::Button button(5, false, false);
  button.setHoldTime(700);
  button.setMulticlickTime(300);
  button.onInit();
  button.addAction(4, mock1, Supla::ON_HOLD);
  button.addAction(1, mock1, Supla::ON_CLICK_1);
  button.addAction(1, mock1, Supla::ON_CLICK_2);
  button.addAction(1, mock1, Supla::ON_CLICK_3);
  button.addAction(1, mock1, Supla::ON_CLICK_4);
  button.addAction(1, mock1, Supla::ON_CLICK_5);
  button.addAction(1, mock1, Supla::ON_CLICK_6);
  button.addAction(1, mock1, Supla::ON_CLICK_7);

  time.advance(1000);
  button.onTimer(); // #2
  time.advance(30); // filtering
  button.onTimer(); // #3 click 1
  time.advance(800); // 
  button.onTimer(); // #4 ON_HOLD
  time.advance(60);
  button.onTimer();
  time.advance(30); // filtering
  button.onTimer(); // #5 release
  time.advance(60); // debounce
  button.onTimer(); // #6 
  time.advance(30); // filtering
  button.onTimer(); // #7 click 2
  time.advance(60); // debounce
  button.onTimer(); // #8 
  time.advance(30); // filtering
  button.onTimer(); // #9 release
  time.advance(500);
  button.onTimer(); // #10 ON_CLICK_2
}

