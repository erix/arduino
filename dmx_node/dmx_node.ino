#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

/*
#include "nRF24L01.h"
#include "RF24.h"
*/

#include "DmxSimple.h"
#include "ledparlight.h"
#include "printf.h"

// Create the LED light
LEDParLight light;

// Structure of our payload
struct payload_t
{
  byte command; // 0-set; 1-get
  byte m; // master channel
  byte r; // reg channel
  byte g; // green channel
  byte b; // blue channel
};

/*
const uint64_t dmx_pipe = 0xF0F0F0F0E1LL;
const uint64_t nexa_pipe =  0xF0F0F0F0D2LL;


RF24 radio(7,8);
*/

void setup() {
  Serial.begin(57600);
  printf_begin();
    
  Serial.println("Starting...");
/*  
  radio.begin();
  
  radio.setChannel(1);
  // Listen on 2 pipes
  radio.openReadingPipe(1, dmx_pipe);
  radio.openReadingPipe(2, nexa_pipe);
  radio.startListening();
  
  radio.printDetails();
*/  
 
  Mirf.cePin = 7;
  Mirf.csnPin = 8;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
  Mirf.setRADDR((byte *)"clie2"); 
  Mirf.setTADDR((byte *)"serv1");
  
  Mirf.payload = sizeof(payload_t);
  Mirf.channel = 90;
  Mirf.config();
  // Set 1MHz data rate - this increases the range slightly
  Mirf.configRegister(RF_SETUP,0x06);

 
  
  light.setChannels(0,0,0,0);
  Serial.println("Ready");
}


void loop() {
/*  
  
  uint8_t pipe_num;
  if ( radio.available(&pipe_num) ) {
    // received data on DMX pipe
    Serial.println("Got packet in pipe, " + pipe_num);

    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &payload, sizeof(payload_t) );     

  	// Delay just a little bit to let the other unit
  	// make the transition to receiver
  	delay(20);
    }
    light.setChannels(payload.m, payload.r, payload.g, payload.b);
    
  }
*/

  if(!Mirf.isSending() && Mirf.dataReady()){
    payload_t payload;
    
    Mirf.getData((byte *)&payload);
    Serial.print(F("Got command: "));
    Serial.println(payload.command);
    
    if( payload.command == 0 ) {
      // Set lights
      // write the received values to the LED lamp
      light.setChannels(payload.m, payload.r, payload.g, payload.b);
    } else if( payload.command == 1 ) {
       payload.m = light.getMaster();
       payload.r = light.getRed();
       payload.g = light.getGreen();
       payload.b = light.getBlue(); 
       
       Mirf.send((byte*)&payload);
       Serial.println(F("Reply sent."));
    }
  }
}
