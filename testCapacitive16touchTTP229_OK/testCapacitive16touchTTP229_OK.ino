// Addressing of the TTP229 LSF

// Slave Address : The identify code for the TTP229-LSF is〝 (1010) 〞. The device address B3 , B2 and
// B1 are fixed〝 111 〞

// Read/Write : The final (eighth) bit of the slave address defines the type of operation to be performed.
// If the R/W bit is〝 1 〞, a read operation is executed. If it is〝 0 〞, a write operation is executed. But
// the TTP229-LSF only accepts read operation.


// Slave Device Addressing
// Device Identifier         Device Address          R/W Bit
// B7 B6 B5 B4               B3 B2 B1                B0
// 1  0  1  0                1  1  1                 R (0)

//////////////////////////////////////////////////////////////////

#include "Wire.h"

#define TTP229_LSF 0x57 // Device address (the addressing without the R/W bit) -> 01010111 = 57
unsigned long previousmillis;

void setup()
{
  Wire.begin(); // wake up I2C bus
  Serial.begin(9600);
  Serial.println("*Sensor Num.            111111");
  Serial.println("              0123456789012345");
  Serial.println("              ----------------");
}

void getTTP229data(byte *a, byte *b)
{

  Wire.requestFrom(TTP229_LSF, 2);    // request 6 bytes from slave device #2

  int dataN = 0;
  while (Wire.available())   // slave may send less than requested
  {
    char c = Wire.read();    // receive a byte as character
    if (dataN == 0) *a = c;
    if (dataN == 1) *b = c;
    dataN++;
  }
}

void showTTP229data()
{
  byte aa, bb = 0;
  getTTP229data(&aa, &bb);
  Serial.print("data TTP229 = ");
  printByte(aa);
  printByte(bb);

  Serial.println(";");
  //delay(100);
}

void loop()
{
  showTTP229data();
  Serial.println(millis() - previousmillis);
  previousmillis = millis();
}



void printByte (byte bytePrint) {
  for (unsigned int mask = 0x80; mask; mask >>= 1) {
    if (mask & bytePrint) {
      Serial.print('1');
    }
    else {
      Serial.print('0');
    }
  }
}
