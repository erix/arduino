#include <SPI.h>
#include <Ethernet.h>
#include <ArdOSC.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include "DmxSimple.h"

class LEDParLight {
public:
    LEDParLight(int base_channel = 1);
        
    ~LEDParLight(){};
    
    int setMaster(int value);
    void setRed(int value);
    void setGreen(int value);
    void setBlue(int value);
    
    int getMaster();
    int getRed();
    int getGreen();
    int getBlue();
    
    int on();
    int off();
    
    bool isOn();

private:
    int _base_channel;
    int _master;
    int _red;
    int _green;
    int _blue;
    int _prevMaster;
};


LEDParLight::LEDParLight(int base_channel) :
    _base_channel(base_channel),
    _master(0),
    _red(0),
    _green(0),
    _blue(0),
    _prevMaster(0)
{
}


int LEDParLight::setMaster(int value) {
    if (value >= 0 && value <= 255) {
        DmxSimple.write(_base_channel, value);
        _master = value;
    }
    
    return _master;
}

void LEDParLight::setRed(int value) {
    if (value >= 0 && value <= 255) {
        DmxSimple.write(_base_channel + 1, value);
        _red = value;
    }        
}

void LEDParLight::setGreen(int value) {
    if (value >= 0 && value <= 255) {
        DmxSimple.write(_base_channel + 2, value);
        _green = value;
    }        
}

void LEDParLight::setBlue(int value) {
    if (value >= 0 && value <= 255) {
        DmxSimple.write(_base_channel + 3, value);
        _blue = value;
    }        
}

int LEDParLight::getMaster() {
    return _master;
}

int LEDParLight::getRed() {
    return _red;
}

int LEDParLight::getGreen() {
    return _green;
}

int LEDParLight::getBlue() {
    return _blue;
}


int LEDParLight::on() {
    return setMaster(_prevMaster);
    // DmxSimple.write(_base_channel, _master);
    // DmxSimple.write(_base_channel + 1, _red);
    // DmxSimple.write(_base_channel + 2, _green);
    // DmxSimple.write(_base_channel + 3, _blue);
}

int LEDParLight::off() {
    _prevMaster = _master;
    return setMaster(0);
    // DmxSimple.write(_base_channel + 1, 0);
    // DmxSimple.write(_base_channel + 2, 0);
    // DmxSimple.write(_base_channel + 3, 0);
}

bool LEDParLight::isOn() {
    return _master > 0;
}


// OSC addresses
#define MASTER_FADER    "/ard/master"
#define RED_FADER       "/ard/red"
#define GREEN_FADER     "/ard/green"
#define BLUE_FADER      "/ard/blue"
#define ON_OFF          "/ard/onoff"
#define REFRESH         "/ard/refresh"

#define LED_R           "/led/red"
#define LED_G           "/led/green"
#define LED_B           "/led/blue"

byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 1, 177 };
int serverPort  = 10000;
int destPort = 12000;

OSCServer server;
OSCClient client;
LEDParLight light;


struct RGB { unsigned char r; unsigned char g; unsigned char b; };
struct RGB led;

void toggle(OSCMessage *_mes) {
    // int value = _mes->getArgInt32(0);
    Serial.println("OnOff"); 
    // Serial.println(value);
    
    int master = 0;
    if (light.isOn()) {
      master = light.off();
    } else {
      master = light.on();
    }
    
    //create new osc message
    OSCMessage newMes1;

    //set destination ip address & port no
    newMes1.setAddress(_mes->getIpAddress(),destPort);
    newMes1.beginMessage(ON_OFF);
    newMes1.addArgInt32(light.isOn() ? 1 : 0);
    client.send(&newMes1);
    
    OSCMessage newMes2;
    newMes2.setAddress(_mes->getIpAddress(),destPort);
    newMes2.beginMessage(MASTER_FADER);
    Serial.println(master/255.0);
    newMes2.addArgFloat(master/255.0);
    client.send(&newMes2);
}


void masterFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    //  if(value!=1) return;
    //Serial.println("Master"); 
    //  Serial.println(value);

    int value = (int) (oscValue * 255);
    light.setMaster(value);
}

void redFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    //  if(value!=1) return;
    //Serial.println("Red"); 
    //Serial.println(value);
    
    int value = (int) (oscValue * 255);
    light.setRed(value);
}

void greenFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    //  if(value!=1) return;
    //Serial.println("Green"); 
    //Serial.println(value);

    int value = (int) (oscValue * 255);
    light.setGreen(value);
}

void blueFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    //  if(value!=1) return;
    //Serial.println("Blue"); 
    //Serial.println(value);

    int value = (int) (oscValue * 255);
    light.setBlue(value);
}

void refreshUI(OSCMessage *_mes) {
    OSCMessage newMes1;
    newMes1.setAddress(_mes->getIpAddress(),destPort);
    newMes1.beginMessage(ON_OFF);
    newMes1.addArgInt32(light.isOn() ? 1 : 0);
    client.send(&newMes1);
    
    OSCMessage newMes2;
    newMes2.setAddress(_mes->getIpAddress(),destPort);
    newMes2.beginMessage(MASTER_FADER);
    newMes2.addArgFloat(light.getMaster()/255.0);
    client.send(&newMes2);
    
    OSCMessage newMes3;
    newMes3.setAddress(_mes->getIpAddress(),destPort);
    newMes3.beginMessage(RED_FADER);
    newMes3.addArgFloat(light.getRed()/255.0);
    client.send(&newMes3);
    
    OSCMessage newMes4;
    newMes4.setAddress(_mes->getIpAddress(),destPort);
    newMes4.beginMessage(GREEN_FADER);
    newMes4.addArgFloat(light.getGreen()/255.0);
    client.send(&newMes4);
    
    OSCMessage newMes5;
    newMes5.setAddress(_mes->getIpAddress(),destPort);
    newMes5.beginMessage(BLUE_FADER);
    newMes2.addArgFloat(light.getBlue()/255.0);
    client.send(&newMes5);        
}


void ledRFader(OSCMessage *_mes) {
  if(Mirf.isSending()) return;
  
  //Serial.println("LED_R");
  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.r != value) {
    led.r = value;
  
    Mirf.setTADDR((byte *)"ledserv1");
    Mirf.send((byte *)&led);
    //Serial.println("Sending");
  }
  //while(Mirf.isSending()){}
  //Serial.println("Finished sending");
}


void ledGFader(OSCMessage *_mes) {
  //Serial.println("LED_G");
  if(Mirf.isSending()) return;
  

  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.g != value) {
    led.g = value;
  
    Mirf.setTADDR((byte *)"ledserv1");
    Mirf.send((byte *)&led);
    //Serial.println("Sending");
  }
  //while(Mirf.isSending()){}
  //Serial.println("Finished sending");
}

void ledBFader(OSCMessage *_mes) {
  if(Mirf.isSending()) return;
  
  //Serial.println("LED_B");
  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.b != value) {
    led.b = value;
  
    Mirf.setTADDR((byte *)"ledserv1");
    Mirf.send((byte *)&led);
    //Serial.println("Sending");
  }
  //while(Mirf.isSending()){}
  //Serial.println("Finished sending");
}

void setup(){ 

    Serial.begin(9600);

    Ethernet.begin(myMac ,myIp); 
    server.begin(serverPort);

    //set callback function & oscaddress
    server.addCallback(ON_OFF, &toggle);
    server.addCallback(MASTER_FADER, &masterFader);
    server.addCallback(RED_FADER, &redFader);
    server.addCallback(GREEN_FADER, &greenFader);
    server.addCallback(BLUE_FADER, &blueFader);
    server.addCallback(REFRESH, &refreshUI);
    server.addCallback(LED_R, &ledRFader);
    server.addCallback(LED_G, &ledGFader);
    server.addCallback(LED_B, &ledBFader);    
    
        
    light.setMaster(0);
    light.setRed(0);
    light.setGreen(0);
    light.setBlue(0);
    
    Mirf.cePin = 7;
    Mirf.csnPin = 8;

    Mirf.spi = &MirfHardwareSpi;
    Mirf.init();
    Mirf.setRADDR((byte *)"ledclie1");    
    Mirf.payload = sizeof(RGB);
    Mirf.config();
    Serial.println(sizeof(RGB));
}

void loop(){
  server.aviableCheck();
}
