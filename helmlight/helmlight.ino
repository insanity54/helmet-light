#include <Adafruit_NeoPixel.h>
#include <avr/interrupt.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);
const uint32_t amber = strip.Color(255, 65, 0);
const uint32_t damber = strip.Color(20, 6, 0);
const uint32_t off = strip.Color(0, 0, 0);
volatile int mode = 0;
const uint8_t times = 15;


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  //Serial.begin(9600);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
//
//  sei();                    // Enable global interrupts
//  EIMSK |= (1 << INT0);     // Enable external interrupt INT0 (pin 2)
//  EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge
  //attachInterrupt(digitalPinToInterrupt(2), changeMode, FALLING); 
}

void loop() {
    //Serial.println(mode);
  
  if (mode == 0) {
    scan(50);
    cls();
    strobe(70);
    cls();
  }

  if (mode == 1) {
    rainbow(20);
  }

  if (mode == 2) {
    colorWipe(strip.Color(0, 0, 255), 50); // Blue
    colorWipe(strip.Color(255, 0, 0), 50); // Red
    colorWipe(strip.Color(255, 255, 255), 50); // White
  }
  
  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
 // colorWipe(strip.Color(0, 255, 0), 50); // Green

//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
////colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
//  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
//  theaterChase(strip.Color(127, 0, 0), 50); // Red
//  theaterChase(strip.Color(0, 0, 127), 50); // Blue
//
//  rainbow(20);
//  rainbowCycle(20);
 // theaterChaseRainbow(50);
}


void cls() {
  for (uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, off);
  }
  strip.show();
}

// Knightrider scan
void scan(uint8_t wait) {
  for(uint8_t t=0; t<times; t++) {
    for(uint16_t i=0; i<((strip.numPixels()*2)-2); i++) {
      
      // go down if i above number of pixels
      if (i > strip.numPixels()-1) {
        strip.setPixelColor((strip.numPixels()-(i-strip.numPixels())-2), amber);
        strip.setPixelColor((strip.numPixels()-(i-strip.numPixels())-1), off);
      }
      
      // go down if i less than number of pixels
      else {
        strip.setPixelColor(i, amber);
        strip.setPixelColor(i-1, off);
      }
  
      // fix for the last pix not being cleared
      if (i == 0) {
        strip.setPixelColor(1, off);
      }
      
      strip.show();
  //    Serial.print(i);
  //    Serial.print(" ");
  //    Serial.print(strip.numPixels());
  //    Serial.print(" ");
  //    Serial.println((strip.numPixels()-(i-strip.numPixels())-2));
      //if (t != (times-1)) {
      delay(wait);
      //}
    }
  }
//  // fix for the last time
//  strip.setPixelColor(1, off);
//  strip.setPixelColor(0, amber);
//  strip.show();
}

void strobe(uint8_t wait) {
  uint16_t d;
  for(uint16_t t=0; t<(times*2); t++) {
    for(uint16_t i=0; i<strip.numPixels(); i++)  {
      //strip.setPixelColor(i, amber);
      if (t % 2) {
        d = wait*6;
        strip.setPixelColor(i, off);
      }
      else {
        d = wait;
        strip.setPixelColor(i, amber);
      }
    }
    strip.show();
    delay(d);
  }
}

// returns the number of the previous pixel
uint16_t prev(uint16_t n) {
//  Serial.print(n);
//  Serial.print(" ");
//  Serial.println(strip.numPixels());

  if (n == 0) {
//    Serial.print(" zero!");
    return strip.numPixels()-1;
  }
  return (n-1);
}

// spin around head
void circle(uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, amber);
    strip.setPixelColor(i-1, damber);
    strip.show();
    delay(wait);
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

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void changeMode() {
    mode += 1;
}
