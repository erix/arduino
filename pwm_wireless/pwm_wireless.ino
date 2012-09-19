/**
 * An Mirf example which copies back the data it recives.
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 *
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <EEPROM.h>

int redPin   = 3;
int greenPin = 6;
int bluePin  = 5;
  


struct RGB { unsigned char r; unsigned char g; unsigned char b; };
struct RGB led;

int memoryAddress = 0;

unsigned long time, elapsed;
boolean newData = false;

void readMemory(int a) {
  led.r = EEPROM.read(a++);
  led.g = EEPROM.read(a++);
  led.b = EEPROM.read(a);
  
  // if all values are 0 then the memory was empty
  if(led.r == 0 && led.g == 0 && led.b == 0) {
    led.r = led.r = led.b = 255;
  }

}

void writeMemory(int a) {
   EEPROM.write(a++, led.r);
   EEPROM.write(a++, led.g);
   EEPROM.write(a, led.b);
}
  
void show() {
  analogWrite(redPin, led.r);
  analogWrite(greenPin, led.g);
  analogWrite(bluePin, led.b);
}

void setup(){
  
  /*
   * Set the SPI Driver.
   */
  Mirf.csnPin = 4;
  Mirf.spi = &MirfHardwareSpi;
  
  /*
   * Setup pins / SPI.
   */
   
  Mirf.init();
  
  /*
   * Configure reciving address.
   */
   
  Mirf.setRADDR((byte *)"ledserv1");
  
  /*
   * Set the payload length to sizeof(unsigned long) the
   * return type of millis().
   *
   * NB: payload on client and server must be the same.
   */
   
  Mirf.payload = sizeof(RGB);
  
  /*
   * Write channel and payload config then power up reciver.
   */
   
  Mirf.config();
  
  
  // Output

  pinMode(redPin,   OUTPUT);   // sets the pins as output
  pinMode(greenPin, OUTPUT);   
  pinMode(bluePin,  OUTPUT);
 
  readMemory(memoryAddress);
  
  show();
}

void loop() {
 
  /*
   * If a packet has been recived.
   *
   * isSending also restores listening mode when it 
   * transitions from true to false.
   */
   
  if(!Mirf.isSending() && Mirf.dataReady()){    
    /*
     * Get load the packet into the buffer.
     */
    
    // save the time when RGB was received
    time = millis();
    newData = true;
    
    Mirf.getData((byte *)&led); 
    
    show();
  }
 
  // save the last RGB value if 5 minutes elapsed from last transmission  
  if(newData) {
    elapsed = millis() - time;
    if (elapsed > 300000) {
      writeMemory(memoryAddress);
      newData = false;
    }
  }
  
}
