#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

#include <JeeLib.h>

#include "oregon.h"
#include "rf12defs.h"


#define PORT 2

OregonDecoderV2 orscV2;

const unsigned long reportPeriod = 900000; // 15 minutes
const unsigned long networkTimeout = 30000; // 30 seconds


unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long pulseCollectInterval = 3600000; // 1 hour
unsigned long lastPulseSent = 0;
unsigned int powerPulses = 0;   // here we collect the pulses

String data; //for services

volatile word pulse;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress myIp(192,168,1,179);
EthernetClient client;

unsigned long time_current, time_sent = 0, time_inside = 0, time_outside = 0;


#if defined(__AVR_ATmega1280__)
void ext_int_1(void) {
#else
ISR(ANALOG_COMP_vect) {
#endif
    static word last;
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
}

String toString(class DecodeOOK& decoder) {
  byte pos;
  const byte* data = decoder.getData(pos);
  String retval("");
  
  for (byte i = 0; i < pos; ++i) {
        retval = retval + String(data[i] >> 4, HEX);
        retval = retval + String(data[i] & 0x0F, HEX);
  }
  return retval;
  
}

void setup () {
    Serial.begin(115200);
    
    // rf12 setup

    rf12_set_cs(8);  // pin 8
    rf12_initialize(myNodeID,freq,network);   //Initialize RFM12 with settings defined above  
    
    Serial.print("Node: "); 
    Serial.print(myNodeID); 
    Serial.print(" Freq: "); 
    if (freq == RF12_433MHZ) Serial.print("433Mhz");
    if (freq == RF12_868MHZ) Serial.print("868Mhz");
    if (freq == RF12_915MHZ) Serial.print("915Mhz"); 
    Serial.print(" Network: "); 
    Serial.println(network);    
      
    Serial.println(F("\n[ookDecoder]"));
    Serial.print(F("report period: "));
    Serial.println(reportPeriod);
    
    Ethernet.begin(mac);
    
    // 433Mhz receiver setup
   
#if !defined(__AVR_ATmega1280__)
    pinMode(13 + PORT, INPUT);  // use the AIO pin
    digitalWrite(13 + PORT, 1); // enable pull-up

    // use analog comparator to switch at 1.1V bandgap transition
    ACSR = _BV(ACBG) | _BV(ACI) | _BV(ACIE);

    // set ADC mux to the proper port
    ADCSRA &= ~ bit(ADEN);
    ADCSRB |= bit(ACME);
    ADMUX = PORT - 1;
#else
   attachInterrupt(1, ext_int_1, CHANGE);

   DDRE  &= ~_BV(PE5);
   PORTE &= ~_BV(PE5);
#endif

}

void loop () {
    static int i = 0;
      
    
    // Oregon reading report
    cli();
    word p = pulse;    
    pulse = 0;
    sei();
    
    if (p != 0) {
        if (orscV2.nextPulse(p)) {
            String reading = toString(orscV2);
            Serial.println(reading);
            
            time_current = millis();
            String model = reading.substring(2,3);
            Serial.println("Model: " + model);
            
            if(isChecksumOK(orscV2)) {
              String hum, temp;
              temp = decodeTemp(orscV2);
              hum = decodeHum(orscV2);

              if( model == "3" ) {
                data = "temp," + temp + "\nhum," + hum;
                if ((time_current - time_outside) > reportPeriod) {
                  sendDataToServer(data, "/streams"); //save data
                  time_outside = millis();
                } else {
                  sendDataToServer(data, "/pusher/streams"); // do not save just announce
                }
                
              } else if( model == "2" ) {
                data = "temp2," + temp + "\nhum2," + hum;
                if ((time_current - time_inside) > reportPeriod) {
                  sendDataToServer(data, "/streams"); // save data
                  time_inside = millis();
                } else {
                  sendDataToServer(data, "/pusher/streams"); // do not save just announce
                }
              }                
            }
            orscV2.resetDecoder();
        }
    }

  // Power reading report  
 if (rf12_recvDone()){    
  if (rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
    
    int node_id = (rf12_hdr & 0x1F);		  //extract nodeID from payload
        
    if (node_id == emonTx_NodeID)  {             //check data is coming from node with the corrct ID
       emontx=*(PayloadTX*) rf12_data;            // Extract the data from the payload 
       Serial.print(F("power: ")); Serial.println(emontx.power); 
       Serial.print(F("pulse: ")); Serial.println(emontx.pulse); 
       Serial.print(F("misc1: ")); Serial.println(emontx.misc1); 
       Serial.print(F("misc2: ")); Serial.println(emontx.misc2); 
       Serial.println(); 
              
       // count the pulses
       powerPulses += emontx.pulse;
       Serial.println("dbg1");
       // prepare data string
       data = "power," + String(emontx.power);
       Serial.println("dbg2");

       // check whether to report the pulses
       if( (millis() - lastPulseSent) >= pulseCollectInterval ) {
         data += "\nWh," + String(powerPulses);
         Serial.println("dbg3");
         if( sendDataToCosm(data) ) {
           sendDataToServer(data, "/streams");
           // we sent it successfully, reset the data
           powerPulses = 0;
           lastPulseSent = millis();
         }
       } else {
         Serial.println("Post to Cosm");
         sendDataToCosm(data);
         sendDataToServer(data, "/streams");   
       }
     }
    }
   }     
}

bool isChecksumOK(class DecodeOOK& decoder) {
  int cs = 0;
  byte pos;
  const byte* data = decoder.getData(pos);
  
  for (byte i = 0; i < pos-2; ++i) { 
      //all but last byte
      cs += data[i] >> 4;
      cs += data[i] & 0x0F;
   }
   cs -= 10;
   
   int csc = ((data[8] >> 4)*16) + (data[8] & 0x0F);   
   return cs == csc;
}

String decodeTemp(class DecodeOOK& decoder) {
  String temp = "";
  byte pos;
  const byte* data = decoder.getData(pos);
  
  if ((data[6] & 0x0F) >= 8) temp += "-";
  temp += ((data[5]>>4) * 10)  + ((data[5] & 0x0F));
  temp += ".";
  temp += (data[4] >> 4);

  return temp;
}

String decodeHum(class DecodeOOK& decoder) {
  String hum = "";
  byte pos;
  const byte* data = decoder.getData(pos);
  hum += ((data[7] & 0x0F)*10)+ (data[4] >> 4);
  return hum;
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

