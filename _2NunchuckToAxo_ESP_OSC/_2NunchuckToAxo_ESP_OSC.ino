// Dual Nunchuck OSC send wifi through ESP-12.
// GAEL JATON 2018
// Public Domain
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include <Wire.h>

int accel_x_axis, accel_y_axis, accel_z_axis ;
byte z_button = 1; // bouton Z
byte c_button = 1; // bouton c
byte buffer[6]; // Buffer contenant les 6 précieux octets qui nous intéresse
byte cnt = 0; // index courant de buffer
byte led = 2; //onboard led gpio2
char ssid[] = "TP-LINK_Extender_2.4GHz"; //"NUMERICABLE-A7A6";          // your network SSID (name)
//char pass[] = "DD0258CE42F43E8AE7EACD4D4D";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 0, 255);     // remote IP . . .255 for Broadcast local network
const unsigned int outPort = 8000;         // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
float last_c_button = 0;
float last_z_button = 0;
unsigned long previousmillis;
//#define DEBUG   ///////////// serial debug

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, NULL);
  //IPAddress ip(192, 168, 0, 7); ////////////////////////////////////////////
  //IPAddress routeur(192, 168, 0, 1);
  //IPAddress subnet(255, 255, 255, 0);
  // WiFi.config(ip, routeur, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

  Wire.begin(4, 5);
  Wire.beginTransmission (0x52); // Séquence d'initialisation nunchuck1 data non-codées
  Wire.write (0xF0);
  Wire.write (0x55);
  Wire.endTransmission ();
  Wire.beginTransmission (0x52);
  Wire.write (0xFB);
  Wire.write (0x00);
  Wire.endTransmission ();

  Wire.begin(13, 12);
  Wire.beginTransmission (0x52); // Séquence d'initialisation nunchuck2 data non-codées
  Wire.write (0xF0);
  Wire.write (0x55);
  Wire.endTransmission ();
  Wire.beginTransmission (0x52);
  Wire.write (0xFB);
  Wire.write (0x00);
  Wire.endTransmission ();
  delay(30);
}

void handshake() // Handshack, merci captain obvious !
{
  Wire.begin(4, 5);
  Wire.beginTransmission (0x52);
  Wire.write (0x00);
  Wire.endTransmission ();
  delay(10);
}
void handshake2() // Handshack, merci captain obvious !
{
  Wire.begin(13, 12);
  Wire.beginTransmission (0x52);
  Wire.write (0x00);
  Wire.endTransmission ();
  delay(10);
}
void parse() {
  int joy_x_axis = buffer[0]; // joystick axe x (0-255)
  int joy_y_axis = buffer[1]; // joystick axe y (0-255)
  int accel_x_axis = buffer[2] * 4 ; // accéléromètre axe x
  int accel_y_axis = buffer[3] * 4 ; // accéléromètre axe y
  int accel_z_axis = buffer[4] * 4 ; // accéléromètre axe z
  if ((buffer[5] >> 0) & 1) z_button = 0;
  else z_button = 1;
  if ((buffer[5] >> 1) & 1) c_button = 0;
  else c_button = 1;
  if ((buffer[5] >> 2) & 1)
    accel_x_axis += 2;
  if ((buffer[5] >> 3) & 1)
    accel_x_axis += 1;
  if ((buffer[5] >> 4) & 1)
    accel_y_axis += 2;
  if ((buffer[5] >> 5) & 1)
    accel_y_axis += 1;
  if ((buffer[5] >> 6) & 1)
    accel_z_axis += 2;
  if ((buffer[5] >> 7) & 1)
    accel_z_axis += 1;
#ifdef DEBUG
  Serial.print ("BLACKNUN");
  Serial.print (joy_x_axis, DEC);
  Serial.print ("\t");
  Serial.print (joy_y_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_x_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_y_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_z_axis, DEC);
  Serial.print ("\t");
  Serial.print (z_button, DEC);
  Serial.print ("\t");
  Serial.print (c_button, DEC);
  Serial.print ("\t");
  Serial.println();
#endif
  OSCBundle bndl; //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
  bndl.add("/axo1").add((float)accel_x_axis / 1023);
  bndl.add("/axo2").add((float)accel_y_axis / 1023);
  //bndl.add("/blackNun/Zaccel").add((float)accel_z_axis / 1023);
  bndl.add("/axo3").add((float)joy_x_axis / 255);
  bndl.add("/axo4").add((float)joy_y_axis / 255);
  bndl.add("/axo5").add((float)c_button);
  bndl.add("/axo6").add((float)z_button);
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();
}
void parse2() {
  int joy_x_axis = buffer[0]; // joystick axe x (0-255)
  int joy_y_axis = buffer[1]; // joystick axe y (0-255)
  int accel_x_axis = buffer[2] * 4 ; // accéléromètre axe x
  int accel_y_axis = buffer[3] * 4 ; // accéléromètre axe y
  int accel_z_axis = buffer[4] * 4 ; // accéléromètre axe z
  if ((buffer[5] >> 0) & 1) z_button = 0;
  else z_button = 1;
  if ((buffer[5] >> 1) & 1) c_button = 0;
  else c_button = 1;
  if ((buffer[5] >> 2) & 1)
    accel_x_axis += 2;
  if ((buffer[5] >> 3) & 1)
    accel_x_axis += 1;
  if ((buffer[5] >> 4) & 1)
    accel_y_axis += 2;
  if ((buffer[5] >> 5) & 1)
    accel_y_axis += 1;
  if ((buffer[5] >> 6) & 1)
    accel_z_axis += 2;
  if ((buffer[5] >> 7) & 1)
    accel_z_axis += 1;
#ifdef DEBUG
  Serial.print ("WITHENUN");
  Serial.print (joy_x_axis, DEC);
  Serial.print ("\t");
  Serial.print (joy_y_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_x_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_y_axis, DEC);
  Serial.print ("\t");
  Serial.print (accel_z_axis, DEC);
  Serial.print ("\t");
  Serial.print (z_button, DEC);
  Serial.print ("\t");
  Serial.print (c_button, DEC);
  Serial.print ("\t");
  Serial.println();
#endif
  OSCBundle bndl; //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
  bndl.add("/axo7").add((float)accel_x_axis / 1023);
  bndl.add("/axo8").add((float)accel_y_axis / 1023);
  //bndl.add("/whiteNun/Zaccel").add((float)accel_z_axis / 1023);
  bndl.add("/axo9").add((float)joy_x_axis / 255);
  bndl.add("/axo10").add((float)joy_y_axis / 255);
  bndl.add("/axo11").add((float)c_button);
  bndl.add("/axo12").add((float)z_button);
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();
}
void loop() {
  while (WiFi.status() != 3 ) {
  digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
  }
  Wire.begin(4, 5);
  Wire.requestFrom (0x52, 6);
  delay(5);
  while (Wire.available ())
  {
    buffer[cnt] = Wire.read();
    cnt++;
  }
  if (cnt >= 5) {
    parse();
  }
  cnt = 0;
  handshake();
  delay(5);

  Wire.begin(13, 12);
  Wire.requestFrom (0x52, 6);
  delay(5);
  while (Wire.available ())
  {
    buffer[cnt] = Wire.read();
    cnt++;
  }
  if (cnt >= 5) {
    parse2();
  }
  cnt = 0;
  handshake2();
  delay (5);
}

