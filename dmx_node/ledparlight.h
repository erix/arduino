class LEDParLight {
public:
    LEDParLight(int base_channel = 1);
        
    ~LEDParLight(){};
    
    void setChannels(int master, int red, int green, int blue);
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

void LEDParLight::setChannels(int master, int red, int green, int blue) {
  setMaster(master);
  setRed(red);
  setGreen(green);
  setBlue(blue);
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

