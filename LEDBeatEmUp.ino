//////////////////////////////////////////////////////////////////////////////////
//  LED (B)Eat Em Up
//  Summary: Eat all the Green dots before the Red dots take over
//  Blue = Player
//  Green = Edible
//  Red = Wall
//  Created By: Allan Murillo
//////////////////////////////////////////////////////////////////////////////////
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_SHIELD
#include <OneSheeld.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS  40
#define PIN        6


//////////////////////////////////////////////////////////////////////////////////
//  Variables
//////////////////////////////////////////////////////////////////////////////////
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 100; // How Fast Pixels Decay (1/10 second)
uint32_t green = pixels.Color(0, 64, 0);
uint32_t red = pixels.Color(64, 0, 0);
uint32_t blue = pixels.Color(0, 0, 128);
uint32_t white = pixels.Color(2, 2, 2);
int player = 0;
int score = 0;


//////////////////////////////////////////////////////////////////////////////////
//  Methods
//////////////////////////////////////////////////////////////////////////////////
void setup() {  
  Reset();

  //  Wait for Python
  while (!Serial.available()) {}
  char c = Serial.read();
  if (c != 'Y') {
    Serial.write(99);
  }

  //  Prepare to Start    
  pixels.show();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Let's Start!");
  delay(5000);
  lcd.clear();    
}

void loop() {
  PixelDecay(); 
  DisplayScreen();   
  pixels.show(); 
  delay(delayval); 
  ParsePythonData(); 
  CheckGamePads();  
  pixels.show();
}


//////////////////////////////////////////////////////////////////////////////////
//  Functions
//////////////////////////////////////////////////////////////////////////////////
void Reset() {    
  OneSheeld.begin();  
  Serial.begin(9600);
   
  //  LiquidCrystal Display  
  lcd.begin(16, 2);  
  lcd.print("LED (B)Eat Em Up");
  lcd.setCursor(0, 1);
  lcd.print("By: Allan");
  score = 0;
  player = 20;

  //  NeoPixels  
  pixels.begin();  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, white); //  Turns on all pixels
  }  
  pixels.show();  //  This sends the information to the pixels
  pixels.setPixelColor(player, blue); 
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
    if (color != white && color != blue && color != red) {      
      uint8_t b = lowByte(color);
      uint8_t r = lowByte(color >> 16);
      uint8_t g = lowByte(color >> 8);
      if (g > 2) {
        g -= 1;
      }
      else {
        r = 64;
        g = 0;
        b = 0;
      }
      pixels.setPixelColor(i, r, g, b);
    }
  }  
}

void ParsePythonData() {  
  while (Serial.available() > 0) {   
    ModifyScore();        
    char hexValue = Serial.read();
    if (pixels.getPixelColor(hexValue) == white) {      
      pixels.setPixelColor(hexValue, green);
    }    
    Serial.write(hexValue);
  }
}

void CheckGamePads() {
  int old = player;
  if (GamePad.isUpPressed()) {
    if((player + 8) < NUMPIXELS){player += 8;}    
  }
  if (GamePad.isDownPressed()) {
    if((player - 8) >= 0){player -= 8;} 
  }
  if (GamePad.isLeftPressed()) {
    if((player + 1) < NUMPIXELS){player += 1;}  
  }
  if (GamePad.isRightPressed()) {
    if((player - 1) >= 0){player -= 1;}  
  }
  if(old!=player){
    OneSheeld.delay(100);
    pixels.setPixelColor(old, white);
    pixels.setPixelColor(player, blue); 
  }  
}
