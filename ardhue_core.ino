#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define DATA_PIN 5
#define NUM_LEDS 60
#define BAUDRATE 9600
#define ADA_BLACK strip.Color(0,0,0)
#define MY_BLACK makeMyColor(0,0,0)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(BAUDRATE);
    strip.begin();
    strip.show();
}

struct MyColor {
  byte r;
  byte g;
  byte b;
};

struct MyColor makeMyColor(byte r, byte g, byte b) {
  struct MyColor toret;
  toret.r = r;
  toret.g = g;
  toret.b = b;
  return toret;
}

uint32_t MyColor_toInt(struct MyColor c) {
  return strip.Color(c.r,c.g,c.b);
}

String inputStr="";
String validCmd="wave";

struct MyColor mcolor0;
struct MyColor mcolor1;
struct MyColor mcolor2;

void filterValidCmd() {
  if (inputStr == "digitalrgb") validCmd=inputStr;
  else if (inputStr == "wave") validCmd=inputStr;
  else if (inputStr == "spectrum") validCmd=inputStr;
  else if (inputStr.substring(0, 6) == "static") { // static 255 000 128
    validCmd="static";
    mcolor0 = makeMyColor(inputStr.substring(7,10).toInt(), inputStr.substring(11,14).toInt(), inputStr.substring(15,18).toInt());
  }
  else if (inputStr == "none") validCmd=inputStr;
  else if (inputStr.substring(0, 8) == "supercar") {
    validCmd="supercar";
    mcolor0 = makeMyColor(inputStr.substring(9,12).toInt(), inputStr.substring(13,16).toInt(), inputStr.substring(17,20).toInt());
  }
  else if (inputStr.substring(0, 4) == "wipe") {
    validCmd = "wipe";
    //length 16 => 1 color
    mcolor0 = makeMyColor(inputStr.substring(5,8).toInt(), inputStr.substring(9,12).toInt(), inputStr.substring(13,16).toInt());
    if (inputStr.length() >= 28) mcolor1 = makeMyColor(inputStr.substring(17,20).toInt(), inputStr.substring(21,24).toInt(), inputStr.substring(25,28).toInt());
    else mcolor1 = mcolor0;
    if (inputStr.length() >= 40) mcolor2 = makeMyColor(inputStr.substring(29,32).toInt(), inputStr.substring(33,36).toInt(), inputStr.substring(37,40).toInt());
    else mcolor2 = mcolor1;
  }
  else if (inputStr.substring(0, 4) == "fade") {
    validCmd = "fade";
    //length 16 => 1 color
    mcolor0 = makeMyColor(inputStr.substring(5,8).toInt(), inputStr.substring(9,12).toInt(), inputStr.substring(13,16).toInt());
    if (inputStr.length() >= 28) mcolor1 = makeMyColor(inputStr.substring(17,20).toInt(), inputStr.substring(21,24).toInt(), inputStr.substring(25,28).toInt());
    else mcolor1 = mcolor0;
    if (inputStr.length() >= 40) mcolor2 = makeMyColor(inputStr.substring(29,32).toInt(), inputStr.substring(33,36).toInt(), inputStr.substring(37,40).toInt());
    else mcolor2 = mcolor1;
  }
  else if (inputStr.substring(0, 7) == "running") {
    validCmd = "running";
    mcolor0 = makeMyColor(inputStr.substring(8,11).toInt(), inputStr.substring(12,15).toInt(), inputStr.substring(16,19).toInt());
  }
  else if (inputStr.substring(0, 6) == "meteor") {
    validCmd = "meteor";
    mcolor0 = makeMyColor(inputStr.substring(7,10).toInt(), inputStr.substring(11,14).toInt(), inputStr.substring(15,18).toInt());
  }
  else {
    Serial.println("ERROR: Invalid command   "+inputStr);
    return;
  }
  Serial.println(inputStr);
}

void getFromSerial() {
  if (Serial.available()) {
    inputStr = Serial.readString();
    filterValidCmd();
  }
}

void setPixel(int Pixel, struct MyColor color) {
  strip.setPixelColor(Pixel, MyColor_toInt(color));
}

void setAll(struct MyColor color) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, color); 
  }
}

void setAllManual(byte r, byte g, byte b) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
}

void setPixelManual(int Pixel, byte r, byte g, byte b) {
  strip.setPixelColor(Pixel, strip.Color(r,g,b));
}

void FadeInOut(struct MyColor color){
  float r, g, b;
      
  for(int k = 0; k < 256; k=k+1) { 
    r = (k/256.0)*color.r;
    g = (k/256.0)*color.g;
    b = (k/256.0)*color.b;
    setAllManual(r,g,b);
    strip.show();
  }
  delay(900);
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*color.r;
    g = (k/256.0)*color.g;
    b = (k/256.0)*color.b;
    setAllManual(r,g,b);
    strip.show();
  }
}

void Supercar(struct MyColor mcolor, int EyeSize, int SpeedDelay, int ReturnDelay){
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(MY_BLACK);
    setPixelManual(i, mcolor.r/10, mcolor.g/10, mcolor.b/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, mcolor); 
    }
    setPixelManual(i+EyeSize+1, mcolor.r/10, mcolor.g/10, mcolor.b/10);
    strip.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(MY_BLACK);
    setPixelManual(i, mcolor.r/10, mcolor.g/10, mcolor.b/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, mcolor);
    }
    setPixelManual(i+EyeSize+1, mcolor.r/10, mcolor.g/10, mcolor.b/10);
    strip.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RunningLights(struct MyColor mcolor, int WaveDelay) {
  int Position=0;
  for(int i=0; i<NUM_LEDS*2; i++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixelManual(i,((sin(i+Position) * 127 + 128)/255)*mcolor.r,
                   ((sin(i+Position) * 127 + 128)/255)*mcolor.g,
                   ((sin(i+Position) * 127 + 128)/255)*mcolor.b);
      }
      strip.show();
      delay(WaveDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    
    strip.setPixelColor(ledNo, r,g,b);
}

void meteorRain(struct MyColor mcolor, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(MY_BLACK);
  
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, mcolor);
      } 
    }
   
    strip.show();
    delay(SpeedDelay);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void loop() {
    // put your main code here, to run repeatedly:
    // Some example procedures showing how to display to the pixels:
    getFromSerial();
    if (validCmd == "digitalrgb") {
      colorWipe(strip.Color(255, 0, 0), 50); // Red
      colorWipe(strip.Color(0, 255, 0), 50); // Green
      colorWipe(strip.Color(0, 0, 255), 50); // Blue
    }
    else if (validCmd == "wave") {
      rainbowCycle(20);
    }
    else if (validCmd == "spectrum") {
      rainbow(20);
    }
    else if (validCmd == "static") {
      setAll(mcolor0);
      strip.show();
    }
    else if (validCmd == "none") {
      setAll(MY_BLACK);
      strip.show();
    }
    else if (validCmd == "supercar") {
      Supercar(mcolor0, 4, 50, 10);
    }
    else if (validCmd == "wipe") {
      colorWipe(MyColor_toInt(mcolor0), 50);
      colorWipe(ADA_BLACK, 50);
      colorWipe(MyColor_toInt(mcolor1), 50);
      colorWipe(ADA_BLACK, 50);
      if (mcolor2.r != mcolor1.r || mcolor2.g != mcolor1.g || mcolor2.b != mcolor1.b) {
        colorWipe(MyColor_toInt(mcolor2), 50);
        colorWipe(ADA_BLACK, 50);
      }
    }
    else if (validCmd == "fade") {
      FadeInOut(mcolor0);
      delay(100);
      FadeInOut(mcolor1);
      delay(100);
      if (mcolor2.r != mcolor1.r || mcolor2.g != mcolor1.g || mcolor2.b != mcolor1.b) {
        FadeInOut(mcolor2);
        delay(100);
      }
    }
    else if (validCmd == "running") {
      RunningLights(mcolor0, 50);
    }
    else if (validCmd == "meteor") {
      meteorRain(mcolor0, 10, 64, true, 30);
    }
}

