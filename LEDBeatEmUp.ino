//////////////////////////////////////////////////////////////////////////////////
//  LED (B)Eat Em Up
//  Summary: Eat all the Green dots before the Red dots take over
//  Blue = Player
//  Green = Edible
//  Red = Not Good
//  Created By: Allan Murillo
//////////////////////////////////////////////////////////////////////////////////
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_SHIELD
#include <OneSheeld.h>
#include <LiquidCrystal.h>
#include <AltSoftSerial.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS  40
#define PIN        6

//////////////////////////////////////////////////////////////////////////////////
//  Variables
//////////////////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal lcd(2, 3, 4, 5, 11, 7);
AltSoftSerial altSerial;  //  Pins 8&9

uint32_t red = pixels.Color(64, 0, 0);
uint32_t white = pixels.Color(2, 2, 2);
uint32_t black = pixels.Color(0, 0, 0);
uint32_t blue = pixels.Color(0, 0, 64);
uint32_t green = pixels.Color(0, 64, 0);
uint32_t orange = pixels.Color(32, 32, 0);

uint32_t playerCol = blue;
int playerPos = 0;
int score = 0;
int delayval = 100;

//////////////////////////////////////////////////////////////////////////////////
//  Methods
//////////////////////////////////////////////////////////////////////////////////
void setup() {    
  Start();

  //  Wait for Python
  while (!altSerial.available()) {}
  char c = altSerial.read();
  if (c != 'Y') {
    altSerial.write(99);
  }
  else {
    altSerial.write(1);
  }

  //OneSheeld.begin();
  //Serial.begin(115200);
  //OneSheeld.disableCallbacksInterrupts();
  //OneSheeld.waitForAppConnection();

  //  Prepare to Start
  pixels.show();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Let's Start!");
  delay(5000);
  lcd.clear();
}

void loop() {
  CheckGamePads();
  PixelDecay();
  DisplayScreen();
  ParsePythonData();
  pixels.show();
}

//////////////////////////////////////////////////////////////////////////////////
//  Functions
//////////////////////////////////////////////////////////////////////////////////
void Start() {
  altSerial.begin(115200);
  
  //  LiquidCrystal Display
  lcd.begin(16, 2);
  lcd.print("LED (B)Eat Em Up");
  lcd.setCursor(0, 1);
  lcd.print("By: Allan");  

  //  NeoPixels
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, white);
  }
  pixels.show();

  //  Player Data
  score = 0;
  playerPos = 20;
  playerCol = blue;
  pixels.setPixelColor(playerPos, playerCol);  
}

void ModifyScore() {
  score += 1;
}

void DisplayScreen() {
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.print(millis() / 1000);
}

void PixelDecay() {
  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t color = pixels.getPixelColor(i);
    if (color == playerCol) {
      continue;
    }
    if (color == black) {
      continue;
    }
    if (color == white) {
      continue;
    }
    if (color == red) {
      pixels.setPixelColor(i, black);
    }
    else {
      uint8_t r = lowByte(color >> 16);
      uint8_t g = lowByte(color >> 8);
      if (g > 2) {
        g -= 1;
        if (g <= 2) {
          g = 0;
          r = 3;
        }
      }
      else if (r > 2) {
        r += 1;
      }
      pixels.setPixelColor(i, r, g, 0);
    }
  }
}

void ParsePythonData() {
  //OneSheeld.delay(30);
  //altSerial.begin(115200);
  altSerial.listen();
  if (altSerial.available() > 0) {
    ModifyScore();
    char hexValue = altSerial.read();
    if (pixels.getPixelColor(hexValue) == white) {
      pixels.setPixelColor(hexValue, green);
    }
    altSerial.write(hexValue);
  }
  //altSerial.end();
  delay(50);
}

void CheckGamePads() {
  int old = playerPos;
  if (GamePad.isUpPressed()) {
    if ((playerPos + 8) < NUMPIXELS) {
      playerPos += 8;
    }
  }
  if (GamePad.isDownPressed()) {
    if ((playerPos - 8) >= 0) {
      playerPos -= 8;
    }
  }
  if (GamePad.isLeftPressed()) {
    if ((playerPos + 1) < NUMPIXELS) {
      playerPos += 1;
    }
  }
  if (GamePad.isRightPressed()) {
    if ((playerPos - 1) >= 0) {
      playerPos -= 1;
    }
  }

  if (GamePad.isGreenPressed()) {
    pixels.setPixelColor(playerPos, green);
  }
  if (GamePad.isRedPressed()) {
    pixels.setPixelColor(playerPos, playerCol);
  }
  if (GamePad.isBluePressed()) {
    pixels.setPixelColor(playerPos, blue);
  }
  if (GamePad.isOrangePressed()) {
    pixels.setPixelColor(playerPos, orange);
  }

  if (old != playerPos) {
    pixels.setPixelColor(old, white);
    pixels.setPixelColor(playerPos, blue);
  }
}
