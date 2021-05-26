#include <M5EPD.h>
#define LGFX_M5PAPER
#include <LovyanGFX.hpp>
#include <max6675.h>
#include "ssr40/ssr40.h"
#include "guiSetup.h"

// define our pins for the thermocouple
#define THERMO_DO_PIN 19
#define THERMO_CS_PIN 26
#define THERMO_CLK_PIN 18

// define our tolerance for our temperature zone
// what this means is we turn off the heat at T+tolerance
// and turn it on at T-tolerance
#define TEMP_TOLERANCE_F 5
#define TEMP_TOLERANCE_C 3

// define our SSR activation pin
#define SSR_ACTIVATION_PIN 33

// declare our thermostat object
//MAX6675 thermo = MAX6675(THERMO_CLK_PIN,THERMO_CS_PIN,THERMO_DO_PIN);

// declare our relay object
SSR40 ssr = SSR40(SSR_ACTIVATION_PIN);

// what mode are we in? I'm defaulting to fahrenheit
bool fahrenheit = true;
// Heating element trackers
bool heatOn = false;
bool shouldHeat = false;
int timeRemaining = 0;
float tempReading =227.288;
int targetTemp = 325;

// Get our graphics going
LGFX gfx;

void setup() {
  // init the M5
  M5.begin();
  M5.RTC.begin();

  // At some point I'd like to read some settings from a config file and not hard code
  gfx.init();
  gfx.setEpdMode(epd_mode_t::epd_fast);
  gfx.setRotation(1);
  gfx.setFont(&fonts::FreeSansBold18pt7b);
  gfx.initTouch();
}



void checkTemperature() {
  if (fahrenheit) {
    //tempReading = thermo.readFahrenheit();
  }else{
    //tempReading = thermo.readCelsius();
  }
}

void switchUnits() {
  fahrenheit = !fahrenheit;
  // update graphic
}

void drawTopBar() {
  gfx.fillRect(0,0,M5EPD_PANEL_W,TOP_BAR_H,TFT_BLACK);
  gfx.setCursor(ELEMENT_PADDING,ELEMENT_PADDING);
  gfx.setTextColor(TFT_WHITE);
  gfx.print("Smoke 'em");
  rtc_time_t time;
  M5.RTC.getTime(&time);
  gfx.setCursor((M5EPD_PANEL_W-ELEMENT_PADDING)-(2*CHARACTER_X_SPACE),ELEMENT_PADDING);
  gfx.printf("%02d:%02d",time.hour, time.min);
  gfx.setCursor(ELEMENT_PADDING, TOP_BAR_H+ELEMENT_PADDING);
}

void writeTempBoxText() {
  int xOffset = gfx.getCursorX();
  int yOffset = gfx.getCursorY();
  gfx.fillRect(xOffset,yOffset, CURRENT_TEMP_BOX_W,CURRENT_TEMP_BOX_H,TFT_LIGHTGREY);
  gfx.setTextColor(TFT_BLACK);
  gfx.setTextSize(2.0);
  gfx.setCursor(xOffset+(CURRENT_TEMP_BOX_W/2)-(CHARACTER_X_SPACE*2.5),yOffset);
  gfx.printf("<-%03.1f",tempReading);
  gfx.setCursor(xOffset+(CURRENT_TEMP_BOX_W/2)-(CHARACTER_X_SPACE*2.5),yOffset+CURRENT_TEMP_BOX_H-(1.5*CHARACTER_Y_SPACE)+ELEMENT_PADDING);
  gfx.printf("->%03d",targetTemp);
  yOffset = yOffset+CURRENT_TEMP_BOX_H+ELEMENT_PADDING;
  gfx.setCursor(xOffset,yOffset);
}

void drawCurrentTempBox() {
  // first draw its uparrow
  int xOffset = gfx.getCursorX();
  int yOffset = gfx.getCursorY();
  gfx.setTextSize(2.0);
  gfx.fillRect(xOffset,yOffset, CURRENT_TEMP_BOX_W,CURRENT_TEMP_CHANGE_BUTTON_Y,TFT_DARKGREY);
  gfx.setTextColor(TFT_WHITE);
  gfx.setCursor(xOffset+(CURRENT_TEMP_BOX_W/2)-(CHARACTER_X_SPACE/2),yOffset);
  gfx.print("^");
  yOffset = yOffset+CURRENT_TEMP_CHANGE_BUTTON_Y+ELEMENT_PADDING;
  gfx.setCursor(xOffset,yOffset);
  // then draw the box
  writeTempBoxText();
  xOffset = gfx.getCursorX();
  yOffset = gfx.getCursorY();
  // then the down arrow
  gfx.fillRect(xOffset,yOffset, CURRENT_TEMP_BOX_W,CURRENT_TEMP_CHANGE_BUTTON_Y,TFT_DARKGREY);
  gfx.setTextColor(TFT_WHITE);
  gfx.setCursor(xOffset+(CURRENT_TEMP_BOX_W/2)-(CHARACTER_X_SPACE/2),yOffset);
  gfx.print("v");
  yOffset = yOffset+CURRENT_TEMP_CHANGE_BUTTON_Y+ELEMENT_PADDING;
  gfx.setCursor(xOffset,yOffset);
  gfx.setTextSize(1.0);
}

void updateScreen() {
  printf("Starting Write");
  gfx.startWrite();
  gfx.fillScreen(TFT_WHITE);
  drawTopBar();
  drawCurrentTempBox();
  printf("Ending Write");
  gfx.endWrite();
}

bool tempTooHigh(){
  if (fahrenheit){
    return tempReading >= targetTemp + TEMP_TOLERANCE_F;
  }
  return tempReading >= targetTemp + TEMP_TOLERANCE_C;
}

bool tempTooLow(){
  if (fahrenheit){
    return tempReading <= targetTemp - TEMP_TOLERANCE_F;
  }
  return tempReading <= targetTemp - TEMP_TOLERANCE_C;
}

void evaluateHeatState() {
  // First check the states if the heat is running
  if (heatOn) {
    if (!shouldHeat) {
      // Heat is on and it shouldn't be for some reason. Turn it off
      ssr.switchOff();
      heatOn = false;
      return;
    }
    // The heat should be on and is, but check if we're in temp range
    checkTemperature();
    if (tempTooHigh()) {
      ssr.switchOff();
      heatOn = false;
      return;
    }
    // the heat is on, but its not too hot yet double check the time
    if (timeRemaining <= 0) {
      ssr.switchOff();
      heatOn = false;
      shouldHeat = false;
      return;
    }
    // nothing to do yet
    return;
  }
  // OK the heat isn't on, should it be?
  else if (shouldHeat) {
    if (timeRemaining >= 0) {
      // there's still time left, check the temp
      checkTemperature();
      if (tempTooLow()) {
        ssr.switchOn();
        heatOn = true;
        return;
      }
    }
    // we're out of time if we reached here, heat should already be off,
    // better safe than sorry I think, unless gpio rules says otherwise
    ssr.switchOff();
    shouldHeat = false;
    return;
  }
  // heats not on, it shouldn't be, we're fine.
}

void loop() {
  // put your main code here, to run repeatedly:
  printf("Updating Screen");
  updateScreen();
  sleep(5);
}