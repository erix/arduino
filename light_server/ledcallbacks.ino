void ledRFader(OSCMessage *_mes) {
  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.r != value) {
    led.r = value;
    sendLEDValues();
  }
}


void ledGFader(OSCMessage *_mes) {
  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.g != value) {
    led.g = value;
    sendLEDValues();
  }
}

void ledBFader(OSCMessage *_mes) {
  float oscValue = _mes->getArgFloat(0);
  int value = (int) (oscValue * 255);
  if(led.b != value) {
    led.b = value;
    sendLEDValues();
  }
}
