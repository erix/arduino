/*
 * IRanalyzer.pde
 * based on http://arduino.cc/playground/Code/InfraredReceivers
 *
 * modified by Darell Tan
 * see http://irq5.wordpress.com/2012/07/27/infrared-remote-control-protocols-part-1/
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#define READ_IR        ((PIND & _BV(2)) >> 2)  // IR pin
#define TIMER_RESET    TCNT1 = 0
#define TIMER_OVF      (TIFR1 & _BV(TOV1))
#define TIMER_CLR_OVF  TIFR1 |= _BV(TOV1)
#define SAMPLE_SIZE    400

#define MAX_SILENCE    5000

unsigned int TimerValue[SAMPLE_SIZE];

void setup() {
  Serial.begin(115200);
  Serial.println("Analyze IR Remote");
  TCCR1A = 0x00;
  TCCR1B = 0x03; // 64 prescaler
  TIMSK1 = 0x00;

  pinMode(2, INPUT);  // IR pin here
}

void loop() {
  int i;


  // reset
  for (i = 0; i < SAMPLE_SIZE; i++)
    TimerValue[i] = 0;

//  Serial.println("Waiting...");
  while(READ_IR == HIGH);
  // transmission started
  // reset timer, clear overflow flag
  TIMER_RESET;
  TIMER_CLR_OVF;

  i = 0;
  bool cont = true; 
  
  char prev_dir = 0;
  
  while( cont && (i < SAMPLE_SIZE) ) {   
    while( READ_IR == prev_dir ) {
      // if the pin has not changed, check for overflow
      if (TIMER_OVF) {
        cont = false;
        break;
      } else if( (READ_IR == HIGH) && ((TCNT1 * 4) > 5000) ){
        // too long silence, the transmission has ended
        cont = false;
        break;
      }
    }
    if (cont) TimerValue[i++] = TCNT1 * 4;
    prev_dir = !prev_dir;
    TIMER_RESET;
    TIMER_CLR_OVF;
        
  }
  
  // end of transmission, print out the readings
  
  int samples = i;
  i = 0;
  byte b = 0; 
  
//  Serial.println("Bit stream detected!");
  while (i < samples) {
    if (TimerValue[i] > 1500) {
//      Serial.println(TimerValue[i]);
      i++;
      continue;
    }
//    Serial.println("---Start byte----");
    if( (i + 16) < samples) {
      b = consumeByte(TimerValue + i);
      print_hex(b, 8);
      i += 16; // consumed 16 samples
    } else {
      //Stop decoding
      break;
    }
  }

//    Serial.print(TimerValue[i++]);
//    Serial.print("\t");
//
//    // newline every 4 values
//    if ((i % 4) == 0)
//      Serial.print("\n");
//  }
//  Serial.println("\nBit stream end!");
  Serial.println();
}

byte consumeByte(unsigned int *values) {
  byte b = 0;
  for(int i = 0; i < 8; i++) {
      unsigned int time0 = values[2 * i];
      unsigned int time1 = values[2 * i + 1];
/*
      Serial.print(time0);
      Serial.print(":");
      Serial.print(time1);
      Serial.print(" -> ");
      
*/    
      if( time0 < 550 ) {
//        Serial.println("Bit start");
        if( time1 < 550) {
//          Serial.print("0");
        } else if( time1 < 1350) {
          b |= 1 << i;
//          Serial.print("1");
        }
      }
  } 
  return b; 
}

void print_hex(int v, int num_places)
{
    int mask=0, n, num_nibbles, digit;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask; // truncate v to specified number of places

    num_nibbles = num_places / 4;
    if ((num_places % 4) != 0)
    {
        ++num_nibbles;
    }

    do
    {
        digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
        Serial.print(digit, HEX);
    } while(--num_nibbles);

}



