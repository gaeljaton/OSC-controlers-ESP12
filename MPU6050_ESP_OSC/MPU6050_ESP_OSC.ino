// MPU-6050 OSC send wifi through ESP-12.
// GAEL JATON 2018
// Public Domain
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include<Wire.h>
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int pression;
byte led = 2; //onboard led gpio2
char ssid[] = "TP-LINK_Extender_2.4GHz"; //"NUMERICABLE-A7A6"; // your network SSID (name)
//char pass[] = "DD0258CE42F43E8AE7EACD4D4D";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 0, 255);     // remote IP . . .255 for Broadcast local network
const unsigned int outPort = 8000;         // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
#define DEBUG

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, NULL); // Connect to WiFi network
  //IPAddress ip(192, 168, 0, 7); ////////////////////////////////////////////
  //IPAddress routeur(192, 168, 0, 1);
  //IPAddress subnet(255, 255, 255, 0);
  // WiFi.config(ip, routeur, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
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

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write((byte)0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);
}
void loop() {
  while (WiFi.status() != 3 ) { //check connection, else blink...
    analogWrite(2, 1023);
    delay(100);
    analogWrite(2, 0);
    delay(100);
  }
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  //Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  //GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  //GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  //GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  pression = analogRead(A0);
#ifdef DEBUG
  Serial.print("vent = "); Serial.println(pression);
  //  Serial.print("AcX = "); Serial.print(AcX);
  //  Serial.print(" | AcY = "); Serial.print(AcY);
  //  Serial.print(" | AcZ = "); Serial.print(AcZ);
  //  Serial.print(" | Tmp = "); Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
  //  Serial.print(" | GyX = "); Serial.print(GyX);
  //  Serial.print(" | GyY = "); Serial.print(GyY);
  //  Serial.print(" | GyZ = "); Serial.println(GyZ);
#endif
  OSCBundle bndl; //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
  bndl.add("axo12").add((float)pression/1024);
  // bndl.add("/mpu/Xaccel").add((float)(AcX+32768) / 65535);
  bndl.add("axo10").add((float)(AcY + 32768) / 65535);
  bndl.add("axo11").add((float)(AcZ + 32768) / 65535);
  //  bndl.add("/mpu/Xgyro").add((float)(GyX+32768) / 65535);
  //  bndl.add("/mpu/Ygyro").add((float)(GyY+32768) / 65535);
  //  bndl.add("/mpu/Zgyro").add((float)(GyZ+32768) / 65535);
  //  bndl.add("/mpu/temp").add((float)Tmp);
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();
  delay(10);
}
