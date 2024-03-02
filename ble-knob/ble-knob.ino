#include "panda00.h"
#include "volume.h"
#include "powerpoint0.h"
#include "Photoshop1.h"
#include <BleKeyboard.h>
#include <ESP32Encoder.h>
BleKeyboard bleKeyboard("BT-Knob", "ESP32-duke24", 74);
#include <SimpleRotary.h>

SimpleRotary rotary(26, 27, 25);
int mode = 1;
int maxModes = 4;

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TFT_GREY 0x5AEB
#define lightblue 0x01E9
#define darkred 0xA041
#define blue 0x5D9B

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

int backlight[5] = {10, 30, 60, 120, 220};
byte b = 1;
int press2 = 0;
int state = 0;

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  bleKeyboard.begin();
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, backlight[b]);

  for (int i = 0; i < b + 1; i++)
    tft.fillRect(210 + (i * 7), 0, 3, 10, blue);
  tft.setCursor(165, 1, 1);
  tft.println("BRIGHT:");
  //tft.pushImage(60, 20,  128, 81, panda00);
  tft.pushImage(80, 20,  80, 80, volume0);
  tft.setCursor(70, 120);
  tft.println("      Volume        ");

}

void loop() {
  byte j;

  // 0 = not pushed, 1 = pushed, 2 = long pushed
  j = rotary.pushType(1000);  // number of milliseconds button has to be pushed for it to be considered a long push.

  if (j == 1) {
    Serial.println("Pushed");
    if (bleKeyboard.isConnected()) {
      //Serial.println("Pressing the Play/Pause.Button via the Bluetooth keyboard");
      //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      changeMode();
      delay(300);

    }
  }

  if (j == 2) {
    Serial.println("Long Pushed");
    if (bleKeyboard.isConnected()) {
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    }
  }
  if (digitalRead(0) == 0) {
    if (bleKeyboard.isConnected()) {
      changeMode();
      delay(300);
      if (mode == 1) {
        tft.pushImage(80, 20,  80, 80, volume0);
        tft.setCursor(70, 120);
        tft.println("      Volume        ");
      }
      if (mode == 2) {
        tft.setCursor(80, 120);
        tft.println("  Cltr + TAB ");
      }
      if (mode == 3) {
        tft.pushImage(80, 20,  80, 74, powerpoint0);
        tft.setCursor(80, 120);
        tft.println(" Presentation ");
      }
      if (mode == 4) {
        tft.pushImage(80, 20,  75, 72, Photoshop1);
        tft.setCursor(60, 120);
        tft.println(" Photoshop Brush size ");
      }
    }
  }


  byte i;

  // 0 = not turning, 1 = CW, 2 = CCW
  i = rotary.rotate();

  if (i == 1) {
    Serial.println("CW");
    if (bleKeyboard.isConnected()) {
      rotateRight();
      /*Serial.println("Volume up");
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);*/
    }
  }

  if (i == 2) {
    Serial.println("CCW");
    if (bleKeyboard.isConnected()) {
      rotateLeft();
      /*Serial.println("Volume down");
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);*/
    }
  }
  if (digitalRead(35) == 0) {
    if (press2 == 0)
    { press2 = 1;    
      tft.fillRect(210, 0, 44, 12, TFT_BLACK);

      b++;
      if (b >= 5)
        b = 0;

      for (int i = 0; i < b + 1; i++)
        tft.fillRect(210 + (i * 7), 0, 3, 10, blue);
      ledcWrite(pwmLedChannelTFT, backlight[b]);
    }
  } else press2 = 0;

}

void changeMode() {
  mode = (mode % maxModes) + 1;
  tft.fillRect(30, 120, 200, 80, TFT_BLACK);
  tft.fillRect(60, 20,  128, 81, TFT_BLACK);
}

void rotateLeft() {
  switch (mode) {
    case 1:
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      break;        
    case 2:
      //Cltr + TAB
      bleKeyboard.write(KEY_LEFT_CTRL);
      bleKeyboard.write(KEY_TAB);
      bleKeyboard.releaseAll();
      break;
    case 3:
      // Lightroom, PowerPoint
      bleKeyboard.write(KEY_UP_ARROW);
      break;
    case 4:
      // Photoshop Brush size
      bleKeyboard.write('&#91;');
      break;
  }
}

void rotateRight() {
  switch (mode) {
    case 1:
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      break;
    case 2:
      //Cltr + SHIFT + TAB
      bleKeyboard.write(KEY_LEFT_CTRL);
      bleKeyboard.write(KEY_LEFT_SHIFT);
      bleKeyboard.write(KEY_TAB);
      bleKeyboard.releaseAll();
      break;
    case 3:
      // Lightroom, PowerPoint
      bleKeyboard.write(KEY_DOWN_ARROW);
      break;
    case 4:
      // Photoshop Brush size
      bleKeyboard.write(']');
      break;
  }
}
