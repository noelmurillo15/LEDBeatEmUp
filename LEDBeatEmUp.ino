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
#define INCLUDE_TERMINAL_SHIELD
#include <EEPROM.h>
#include <OneSheeld.h>
#include <LiquidCrystal.h>
#include <AltSoftSerial.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS  40
#define BUZZERPIN  12
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
int delayval = 90;

//////////////////////////////////////////////////////////////////////////////////
//  Methods
//////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(BUZZERPIN, OUTPUT);
  ///////////////////////////////////
  //  Initialize
  ///////////////////////////////////
  Start();
  tone(BUZZERPIN, 'a', 250);

  ///////////////////////////////////
  //  Wait for Python
  ///////////////////////////////////
  while (!altSerial.available()) {}
  char c = altSerial.read();
  if (c != 'Y') { //  '\r' ascii - Enter
    altSerial.write(7);
    altSerial.flush();
  }
  else {
    altSerial.write(1);
    altSerial.flush();
    tone(BUZZERPIN, 'a', 250);
  }

  ///////////////////////////////////
  //  Wait for OneSheeld (Bluetooth)
  ///////////////////////////////////
  //OneSheeld.begin();

  ///////////////////////////////////
  //  Prepare Displays
  ///////////////////////////////////
  pixels.show();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Let's Start!");
  tone(BUZZERPIN, 'a', 250);
  delay(5000);
  lcd.clear();
}

void loop() {
  DisplayScreen();
  CheckGamePads();
  PixelDecay();
  ParsePythonData();
  pixels.show();
  delay(delayval);
}

//////////////////////////////////////////////////////////////////////////////////
//  Main Functions
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//  Start -
//////////////////////////////////////////////////////////////
void Start() {
  altSerial.begin(115200);

  ///////////////////////////////////
  //  Arduino Flash Memory Read
  ///////////////////////////////////
  int addr = 0;
  EEPROM.get(addr, score);

  ///////////////////////////////////
  //  Liquid Crystal Display
  ///////////////////////////////////
  lcd.begin(16, 2);
  lcd.print("LED (B)Eat Em Up");
  lcd.setCursor(0, 1);
  if (score == 0) {
    lcd.print("By: Allan");
  }
  else {
    lcd.print("High Score: ");
    lcd.print(score);
  }

  ///////////////////////////////////
  //  NeoPixels - resets color to white
  ///////////////////////////////////
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, white);
  }
  pixels.show();

  ///////////////////////////////////
  //  Player Data
  ///////////////////////////////////
  score = 0;
  delayval = 100;
  playerPos = 20;
  playerCol = blue;
  pixels.setPixelColor(playerPos, playerCol);
}
//////////////////////////////////////////////////////////////
//  Python -
//////////////////////////////////////////////////////////////
void ParsePythonData() {
  altSerial.listen();
  while (altSerial.available() > 0) {
    char hexValue = altSerial.read();
    if (pixels.getPixelColor(hexValue) == white) {
      ModifyScore();
      pixels.setPixelColor(hexValue, green);
      altSerial.write(1);
      altSerial.flush();
      return;
    }
    if (CheckColorCount(white) > 10) {
      int r = random(0, 39);
      if (pixels.getPixelColor(r) == white) {
        pixels.setPixelColor(r, green);
        altSerial.write(2);
        altSerial.flush();
        ModifyScore();
        if (delayval > 1)
          delayval--;
        return;
      }
    }
    if (CheckColorCount(black) <= 38) {
      pixels.setPixelColor(getFirstColor(white), green);
      ModifyScore();
      altSerial.write(2);
      altSerial.flush();
      if (delayval > 1)
        delayval--;
      return;
    }
  }

  if (CheckColorCount(black) > 38) {
    Quit();
  }
}

//////////////////////////////////////////////////////////////
//  Pixels -
//////////////////////////////////////////////////////////////
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
  pixels.show();
}
//////////////////////////////////////////////////////////////
//  Gamepad (Bluetooth Smartphone) -
//////////////////////////////////////////////////////////////
void CheckGamePads() {
  ///////////////////////////////////
  //  GamePad Color Buttons
  ///////////////////////////////////
  if (GamePad.isRedPressed()) {
    playerCol = red;
  }
  if (GamePad.isBluePressed()) {
    playerCol = blue;
  }
  if (GamePad.isGreenPressed()) {
    playerCol = green;
  }
  if (GamePad.isOrangePressed()) {
    playerCol = orange;
  }

  ///////////////////////////////////
  //  GamePad D-Pad
  ///////////////////////////////////
  int old = playerPos;
  if (GamePad.isUpPressed()) {
    if ((playerPos + 8) < NUMPIXELS && pixels.getPixelColor(playerPos + 8) != black) {
      playerPos += 8;
    }
  }
  if (GamePad.isDownPressed()) {
    if ((playerPos - 8) >= 0 && pixels.getPixelColor(playerPos - 8) != black) {
      playerPos -= 8;
    }
  }
  if (GamePad.isLeftPressed()) {
    if ((playerPos + 1) < NUMPIXELS && pixels.getPixelColor(playerPos + 1) != black) {
      playerPos += 1;
    }
  }
  if (GamePad.isRightPressed()) {
    if ((playerPos - 1) >= 0 && pixels.getPixelColor(playerPos - 1) != black) {
      playerPos -= 1;
    }
  }

  ///////////////////////////////////
  //  Input Change
  ///////////////////////////////////
  pixels.setPixelColor(playerPos, playerCol);
  if (old != playerPos) {
    OneSheeld.delay(100);
    pixels.setPixelColor(old, white);
  }
  pixels.show();
}
//////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
//////////////////////////////////////////////////////////////////////////////////
void Quit() {
  altSerial.write(7);  //  Quit code
  altSerial.flush();

  //  Save score to memory
  int addr = 0;
  EEPROM.put(addr, score);
  tone(BUZZERPIN, 'a', 100);
  delay(5000);

  //  Soft Reset
  asm volatile ("  jmp 0");
}
//////////////////////////////////////////////////////////////
//  Score -
//////////////////////////////////////////////////////////////
void ModifyScore() {
  score += 1;
}
//////////////////////////////////////////////////////////////
//  DisplayScreen -
//////////////////////////////////////////////////////////////
void DisplayScreen() {
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.print(millis() / 1000);
}
//////////////////////////////////////////////////////////////
//  Pixels -
//////////////////////////////////////////////////////////////
int getFirstColor(uint32_t col) {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (pixels.getPixelColor(i) == col) {
      return i;
    }
  }
  return 40;
}
int CheckColorCount(uint32_t col) {
  int count = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    if (pixels.getPixelColor(i) == col) {
      count++;
    }
  }
  return count;
}
