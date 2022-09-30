int numZONES = 4;
struct Time {
  unsigned int hour;
  unsigned  int minute;
  unsigned  int second;
} T;
struct Settings {
  float tempMAX;
  float humMAX;
  float humMIN;
  float Auto;
  struct Time wateringON;
  struct Time wateringOFF;
};
struct Zone {
  /////////working sensors///////
  float temperature;
  float humidity;
  ///////for settings///////
  struct Settings SET_ZONE;
  //////working auto watering////////
  boolean FLAGwatering;
  float Last_time;
  boolean FLAGshoot_time;
};
///////////////////flags for Sell comand //////////////////////////
bool FlagNZONESset;
bool FlagTHset;
bool FlagWATERING_HOURSset;
///////////////////flags for WORKING //////////////////////////
bool FlagON;
/////////////////////////  EEPROM  ///////////////////////////////
#include <EEPROM.h>
bool FlagREAD_ZONES_EEPROM = true;
#define eeAddress_startZONES 6
void setup() {
  Serial.begin(9600);
}
void loop() {
  struct Zone ZONE[4];
  ////////////READ EEPROM for ZONES////////////////
  if (FlagREAD_ZONES_EEPROM == true) {
    READee_ZONES(ZONE);
    FlagREAD_ZONES_EEPROM = false;
  }
  for (int x = 0; x < numZONES; x++) {
    Serial.print("\n");
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" tempMAX :");
    Serial.println(ZONE[x].SET_ZONE.tempMAX);
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" humMAX :");
    Serial.println(ZONE[x].SET_ZONE.humMAX);
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" humMIN :");
    Serial.println(ZONE[x].SET_ZONE.humMIN);
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" Auto :");
    Serial.println(ZONE[x].SET_ZONE.Auto);
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" WateringON :");
    Serial.print(ZONE[x].SET_ZONE.wateringON.hour); Serial.print(":");
    Serial.println(ZONE[x].SET_ZONE.wateringON.minute);
    Serial.print("ZONE "); Serial.print(x + 1); Serial.print(" WateringOFF :");
    Serial.print(ZONE[x].SET_ZONE.wateringOFF.hour); Serial.print(":");
    Serial.println(ZONE[x].SET_ZONE.wateringOFF.minute);
  }
}
void READee_ZONES(struct Zone * ptrZONE )//work 16:08
{
  int sizeAddress = sizeof(ptrZONE->SET_ZONE);
  for (int x = 0; x < numZONES; x++) {
    EEPROM.get(eeAddress_startZONES + (x * sizeAddress), ((ptrZONE + x)->SET_ZONE) );
  }
}
