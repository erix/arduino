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

int redPin   = 3;
int greenPin = 6;
int bluePin  = 5;
  


struct RGB { unsigned char r; unsigned char g; unsigned char b; };
struct RGB led;

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
  analogWrite(redPin, 255);
  analogWrite(greenPin, 255);
  analogWrite(bluePin, 255);
}

void loop(){
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
     
    Mirf.getData((byte *)&led);
  //  analogWrite(redPin, 255);
  //  delay(100);
  //  analogWrite(redPin, 0);
  //  delay(100);
    
    
    analogWrite(redPin, led.r);
    analogWrite(greenPin, led.g);
    analogWrite(bluePin, led.b);
  }
}
