#include <Wire.h>
byte code=B11111111;
void setup() {
  Wire.begin();
  Wire.beginTransmission(0x38);
  Wire.write(0xff);
  Wire.endTransmission();
}
void loop() {
  PCF8574LOW(1);
  PCF8574LOW(2);
  delay(1000);
  PCF8574HIGH(1);
  PCF8574HIGH(2);
}
void PCF8574LOW(unsigned int Gate)
{
  code = code | ((B11111111) - ((B00000001) << Gate));
  Wire.beginTransmission(0x38);
  Wire.write(code);
  Wire.endTransmission();

}
void PCF8574HIGH(unsigned int Gate)
{
  code = code & (B00000001) << Gate;
  Wire.beginTransmission(0x38);
  Wire.write(code);
  Wire.endTransmission();
}
