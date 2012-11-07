void masterFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    dmxValues.m = (int) (oscValue * 255);
    sendDMXValues();
}

void redFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    dmxValues.r = (int) (oscValue * 255);
    sendDMXValues();
}

void greenFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    dmxValues.g = (int) (oscValue * 255);
    sendDMXValues();
}

void blueFader(OSCMessage *_mes) {
    float oscValue = _mes->getArgFloat(0);
    dmxValues.b = (int) (oscValue * 255);
    sendDMXValues();
}

void refreshUI(OSCMessage *_mes) {
  Serial.println("Refresh");
  getDMXValues();
  sendRefreshUI(_mes->getIpAddress()); 
}

