//
//  nanoEvery.ino
//
//  Author:          Michael Bernhard
//  Available from:  https://github.com/mchlbrnhrd/nanoEvery
//
//  Description:     Example code for Arduino Nano Every (ATMEGA4809)
//                   * timer - real time counter (RTC)
//                   * watchdog
//
// GNU General Public License v3.0
//
// Copyright (C) 2022  Michael Bernhard
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include <avr/wdt.h> // required for watchdog

int counter=0;

void setup() {
  
  wdt_reset(); // call this at the beginning of setup()
  wdt_disable(); // call this at the beginning of setup()
  delay(1000);
  wdt_reset();

  Serial.begin(9600);
  counter=0;

  
  //============================
  // watchdog
  //============================
  #ifdef ARDUINO_ARCH_MEGAAVR
  if (RSTCTRL.RSTFR & RSTCTRL_WDRF_bm) {
    // Arudino was started via watchdog
    Serial.println(F("Arduino was started via watchdog"));
    // error handling can be done here
    // ...
  }
  RSTCTRL.RSTFR |= RSTCTRL_WDRF_bm ;
  // set watchdog timer to 8 seconds
  wdt_enable(WDT_PERIOD_8KCLK_gc);
  #endif


  //============================
  // setup RTC timer
  //============================
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
  while (RTC_CTRLBUSY_bm & RTC.PITSTATUS); // register busy check
  RTC.PITINTCTRL = RTC_PI_bm; // enable of period interrupts
  while (RTC_CTRLABUSY_bm & RTC.STATUS); // register busy check
  // here: set trigger every 128 clock cycle
  // @32768Hz => 128/32768Hz = 1/256 second
  //          => enable period interrupt bit at every 1/256 second
  RTC.PITCTRLA = RTC_PERIOD_CYC128_gc  | RTC_PITEN_bm;
  
}

void loop() {
  Serial.println(counter++);
  delay(1000);
  wdt_reset(); // call wdt_reset in less then 8 seconds
               // remove wdt_reset and see what happens
}


//============================
// timer interrupt
//============================
ISR(RTC_PIT_vect) {
  // your timer interrupt code
  //...

  RTC.PITINTFLAGS = RTC_PI_bm; // reset interrupt
}
