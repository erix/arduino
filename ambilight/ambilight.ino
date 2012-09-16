#include "SPI.h"
#include "Adafruit_WS2801.h"

#define dataPin 3
#define clockPin 2

// the number of LED/WS2801 pairs; must be in the range [1, 2**8)
#define stripLength 70

// specified under `rate` in the `[device]` section of /etc/boblight.conf
#define serialRate 38400

// boblightd sends a prefix (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

void setup() {
  Serial.begin(serialRate);
  // initialise our LED strip
  strip.begin();
  // turn all the other pixels on so we know they're working
  for(uint16_t i = 0; i < stripLength; ++i)
    strip.setPixelColor(i, Color(0, 0, 0));
  colorWipe(Color(0, 20, 200), 50);
}

void loop() {
  // wait until we see the prefix
  for(byte i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) ;;
    // look for the next byte in the sequence if we see the one we want
    if(prefix[i] == Serial.read()) continue;
    // otherwise, start over
    i = 0;
    goto waitLoop;
  }
  // read the transmitted data
  for (uint8_t i = 0; i < stripLength; i++) {
    byte r, g, b;
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    strip.setPixelColor(i, Color(b, g, r));
  }
  strip.show();
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  int count = strip.numPixels();
  
  for (i=0; i < count / 2; i++) {
      strip.setPixelColor(i, c);
      strip.setPixelColor(count-i-1, c);
      strip.show();
      delay(wait);
  }
}
