# OSC-controlers-ESP12
various diy open_sound_control (OSC) controlers based on ESP8266 (esp12)<br />
<br />
Some home made osc controlers with accelerometer, breath control (pressure sensor) and other sensors
Required :<br />
*ESP 12 board (nodemcu, wemos, ESPino...) or ESP32<br />
*I2C sensors : MPU-6050 (accel), nunchuck nintendo, pressure sensor, TTP229 (16 touch sensor)...<br />
*analog sensors : pressure sensor, switch, potentiometer, LDR...<br />
*arduino IDE with esp8266/32 board manager installed https://www.instructables.com/id/Programming-the-ESP8266-12E-using-Arduino-software/ <br />
*OSC arduino library  https://github.com/CNMAT/OSC<br />
*wifi router (I use dedicated one internet offline) <br />
*OSC compatible synth (Processing, PureData, MaxMsp...)<br />

Wiring diagram for breath controler + 2axis tilt (MPU-6050 accelerometer)<br />
![alt text](https://github.com/gaeljaton/OSC-controlers-ESP12/blob/master/BreathWiring.jpg)
