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
bool FlagNZONESset = false;
bool FlagTHset = false;
bool FlagWATERING_HOURSset = true;
///////////////////flags for WORKING //////////////////////////
bool FlagON = false;
/////////////////////////  EEPROM  ///////////////////////////////
#include <EEPROM.h>
bool FlagREAD_ZONES_EEPROM = true;
#define eeAddress_startZONES 6
void setup() {
  Serial.begin(9600);
  struct Zone ZONE[4];
  for (int x = 0; x < numZONES; x++) {
    ZONE[x].SET_ZONE.tempMAX = 40;
    ZONE[x].SET_ZONE.humMAX = 85;
    ZONE[x].SET_ZONE.humMIN = 25;
    ZONE[x].SET_ZONE.Auto = 45;
    ZONE[x].SET_ZONE.wateringON = {18, 30, 00};
    ZONE[x].SET_ZONE.wateringOFF = {22, 45, 10};
  }
  /////////////////Save settings of ZONES into EEPROM//////////////////
  WRITEee_ZONES(ZONE);
}
void loop() {
  // put your main code here, to run repeatedly:

}
/////////////////////////////////////////EEPROM_WRITE/////////////////////////////////////
void WRITEee_ZONES(struct Zone * ptrZONE )//work 16:08
{
  int sizeAddress = sizeof(ptrZONE->SET_ZONE);
  for (int x = 0; x < numZONES; x++) {
    EEPROM.put(eeAddress_startZONES + (x * sizeAddress), ((ptrZONE + x)->SET_ZONE) );
  }
  Serial.println("ZONES SAVED");
}
