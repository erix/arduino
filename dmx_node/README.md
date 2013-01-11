Sketch for controlling DMX lights with radio signals
 
## DMX
The Arduino shield is built using the [DMX shields documentation](http://playground.arduino.cc/DMX/DMXShield) on Arduino Playground. The software which drives the DMX is [DMXSimple](http://code.google.com/p/tinkerit/wiki/DmxSimple).
I'm using 5 channel [LED Par Can lights](http://www.amazon.com/American-Mega-Par-Profile-RGB/dp/B005ES7VSW/ref=pd_cp_MI_0), 4 of them, as mood light chained together with 3pin DMX cable. This is a good RGB light [American DJ Mega Par]

## RF Link
For the RF link I have used the nRF24l01 chip. The software whish drives the RF chip is [Mirf](https://github.com/aaronds/arduino-nrf24l01)

Here is the Fritzing drawing
![Schema](https://dl.dropbox.com/u/149684/img/dmx_bb.png)
