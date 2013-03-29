IR code analyzer sketch which prints out the HEX values of the remote control codes of Mitshubishi air conditioner. Probably this will work also for other LCD air conditioning remote controls.

To build you can use
  * the Arduino IDE or
  * the included Makefile after installing the [Arduino CLI](http://mjo.tc/atelier/2009/02/arduino-cli.html) utility

To build with the Makefile please check the `BOARD_TAG` and `ARDUINO_PORT` and set it correctly for your setup. After that just type:
```bash
make
make upload
make monitor
```

Here is the schema for building the electronics:

![Schema](https://dl.dropbox.com/u/149684/img/ir_analyzer_bb.jpg)
