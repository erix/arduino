#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Ethernet.h>
#include <ArdOSC.h>
#include <EthernetBonjour.h>

// networking setup values
byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 1, 177 };
int serverPort  = 10000;
int destPort = 12000;

OSCServer oscServer;
OSCClient oscClient;

//byte dmx_pipe[] = {0xf0, 0xf0, 0xf0, 0xf0, 0xe1};

// OSC addresses
#define MASTER_FADER    "/ard/master"
#define RED_FADER       "/ard/red"
#define GREEN_FADER     "/ard/green"
#define BLUE_FADER      "/ard/blue"
//#define ON_OFF          "/ard/onoff"
#define REFRESH         "/ard/refresh"

#define LED_R           "/led/red"
#define LED_G           "/led/green"
#define LED_B           "/led/blue"

// Structure of our payload
struct dmx_light_t
{
  byte command; // 0 - set; 1 - get
  byte m; // master channel
  byte r; // reg channel
  byte g; // green channel
  byte b; // blue channel
};

struct led_light_t
{
  byte command; // 0 - set; 1 - get
  byte r; // reg channel
  byte g; // green channel
  byte b; // blue channel
};

dmx_light_t dmxValues = {0,0,0,0,0};
led_light_t led = {0,0,0,0};

void setup() {
  Serial.begin(57600);
  
  // Configure the radio
  Mirf.cePin = 7;
  Mirf.csnPin = 8;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
  Mirf.setRADDR((byte *)"serv1");
  Mirf.channel = 90;
  Mirf.config();
  
  // Set 1MHz data rate - this increases the range slightly
  Mirf.configRegister(RF_SETUP,0x06);
  
  /* Radio channel setup parameters
  byte rf;
  Mirf.readRegister(RF_SETUP, &rf, 1);
  Serial.println(rf, BIN);
  */
  
  Ethernet.begin(myMac ,myIp); 
  oscServer.begin(serverPort);
  
  //set callback function & oscaddress
  //server.addCallback(ON_OFF, &toggle);  
  oscServer.addCallback(MASTER_FADER, &masterFader);
  oscServer.addCallback(RED_FADER, &redFader);
  oscServer.addCallback(GREEN_FADER, &greenFader);
  oscServer.addCallback(BLUE_FADER, &blueFader);
  oscServer.addCallback(REFRESH, &refreshUI);
  
  oscServer.addCallback(LED_R, &ledRFader);
  oscServer.addCallback(LED_G, &ledGFader);
  oscServer.addCallback(LED_B, &ledBFader);
  
  EthernetBonjour.begin("arduino");
  EthernetBonjour.addServiceRecord("Arduino Light Server._osc",
    serverPort,
    MDNSServiceUDP);
    
  EthernetBonjour.setServiceFoundCallback(serviceFound);
}

void loop() {
  oscServer.aviableCheck();
  EthernetBonjour.run();
  if (!EthernetBonjour.isDiscoveringService()) {
      EthernetBonjour.startDiscoveringService("_osc",
                                              MDNSServiceUDP,
                                              10000);
  }
}

void sendDMXValues() {
  Serial.println("Sending dmx data");
  if(Mirf.isSending()) return;
  Mirf.payload = sizeof(dmx_light_t);
  Mirf.config();
  dmxValues.command = 0;
//  Mirf.setTADDR(dmx_pipe);
  Mirf.setTADDR((byte *)"clie2");
  Mirf.send((byte *)&dmxValues);
}

void sendLEDValues() {
  Serial.println("Sending led data");
  if(Mirf.isSending()) return;
  Mirf.payload = sizeof(led_light_t);
  Mirf.config();
  led.command = 0;
  Mirf.setTADDR((byte *)"clie1");
  Mirf.send((byte *)&led);
}

void getDMXValues() {
  Serial.println("Getting dmx data");
  if(Mirf.isSending()) return;
  Mirf.payload = sizeof(dmx_light_t);
  Mirf.config();
  dmxValues.command = 1;
//  Mirf.setTADDR(dmx_pipe);
  Mirf.setTADDR((byte *)"clie2");
  Mirf.send((byte *)&dmxValues);
  
  while( Mirf.isSending() ); // wait to finish the sending
  
  unsigned long time = millis();
  while(!Mirf.dataReady()){ // wait for the response
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 1000 ) {
      Serial.println(F("Timeout on response from dmx node!"));
      return;
    }
  }
  Serial.println(F("Got response"));
  Mirf.getData((byte *) &dmxValues);
}

void getLEDValues() {
  Serial.println("Getting LED data");
  if(Mirf.isSending()) return;
  Mirf.payload = sizeof(led_light_t);
  Mirf.config();
  led.command = 1;
  Mirf.setTADDR((byte *)"clie1");
  Mirf.send((byte *)&led);
  
  while( Mirf.isSending() ); // wait to finish the sending
  
  unsigned long time = millis();
  while(!Mirf.dataReady()){ // wait for the response
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 1000 ) {
      Serial.println(F("Timeout on response from LED node!"));
      return;
    }
  }
  Serial.println(F("Got response"));
  Mirf.getData((byte *) &led);  
}

void sendRefreshUI(uint8_t *ip) {
    //create new osc message
    OSCMessage newMes;

    newMes.setAddress(ip,destPort);  
    newMes.beginMessage(MASTER_FADER);
    newMes.addArgFloat(dmxValues.m/255.0);
    oscClient.send(&newMes);
    newMes.flush();
    
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(RED_FADER);
    newMes.addArgFloat(dmxValues.r/255.0);
    oscClient.send(&newMes);
    newMes.flush();
    
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(GREEN_FADER);
    newMes.addArgFloat(dmxValues.g/255.0);  
    oscClient.send(&newMes);
    newMes.flush();
  
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(BLUE_FADER);
    newMes.addArgFloat(dmxValues.b/255.0); 
    oscClient.send(&newMes);
    newMes.flush();
    
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(LED_R);
    newMes.addArgFloat(led.r/255.0);
    oscClient.send(&newMes);
    newMes.flush();
    
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(LED_G);
    newMes.addArgFloat(led.g/255.0);  
    oscClient.send(&newMes);
    newMes.flush();
  
    newMes.setAddress(ip,destPort);
    newMes.beginMessage(LED_B);
    newMes.addArgFloat(led.b/255.0); 
    oscClient.send(&newMes);    
}


