//////////////////////////////TimeandDate///////////////////////////////
#include <TimerOne.h>
struct Date {
  unsigned int day;
  unsigned  int month;
  unsigned  int year;
} D;
struct Time {
  unsigned int hour;
  unsigned  int minute;
  unsigned  int second;
} T;
////////////////////for_liquidcrystal_LCD//////////////////////////////
#include <LiquidCrystal.h>
//initialize the library with the numbers of the interface pins:
LiquidCrystal lcd(2, 3, 4, 5, 7, 8);
//////////////////////////// for_Sensors//////////////////////////////
enum sensors {temperature, humidity, zone1y2, zone3y4, allzones};
////////////////////////for_driverPCF8574 I2C///////////////////////////
#include <Wire.h>
byte code = 0xff;
////////////////////////////////Zones//////////////////////////////////
int numZONES;
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
bool FlagNZONESset = true;
bool FlagTHset = true;
bool FlagWATERING_HOURSset = true;
///////////////////flags for WORKING //////////////////////////
bool FlagON = true;
bool Flagprintset = true;
/////////////////////////  EEPROM  ///////////////////////////////
#include <EEPROM.h>
//EEPROM_mannagering:
bool FlagREAD_ZONES_EEPROM = true;
#define TOP_Address EEPROM.length()
#define Boolean 0
//EEPROM Address:
#define eeAddress_numZONES 0
#define eeAddress_FlagNZONESset 2
#define eeAddress_FlagTHset 3
#define eeAddress_FlagWATERING_HOURSset 4
#define eeAddress_FlagON 5
#define eeAddress_startZONES 6
/*numZONES is a int variable of 16bits. therefore:
  address of numZONES is 0 and 1.
  FlagNZONESset, FlagTHset and FlagWATERING_HOURSset
  are a bool variable of 8 bits. therefore:
  address of FlagNZONESset is 2.
  address of FlagTHset is 3.
  address of FlagWATERING_HOURSsetis 4.
  FlagON is a bool variable of 8 bits. therefore:
  address of FlagON is 5 */
void setup() {
  // initialize the LED pin as an output.
  Serial.begin(9600);
  eeClean(0, TOP_Address);
  if (eeBussy(eeAddress_numZONES, TOP_Address) == true) {
    Serial.println("eeprom ocupada");
  } else {
    Serial.println("eeprom limpia");
  }
}

void loop() {
  ////Empty
}
bool eeBussy(int Start, int Finish) //work 14:00
{
  int EEPROM_free = 0;
  for (int i = Start; i < Finish ; i++)
  {
    if (EEPROM.read(i) == 0)EEPROM_free++;
  }
  Serial.print("\nsize interval: "); Serial.print(Finish - Start); Serial.println(" bytes");
  Serial.print("FREE: "); Serial.print(EEPROM_free); Serial.println(" bytes\n");
  if (EEPROM_free == (Finish - Start) )return false;
  else return true;
}
void eeClean(int eeAddress_start, int VAL)//work 14:00
{
  if (VAL == TOP_Address) {
    for (int i = 0; i < TOP_Address; i++) {
      EEPROM.write(i, 0);
    }
  }
  if (!VAL) {
    //if VAL=ALL_eeAdress clean all eeprom
    for (int i = eeAddress_start ; i < eeAddress_start + sizeof(VAL) ; i++) {
      EEPROM.write(i, 0);
    }
  } else {
    //if VAL=Boolean is same as VAL=0 clean only one Address
    EEPROM.write(eeAddress_start, 0);
  }
  Serial.println("\nREMOVED\n");
}
void eeClean_ZONE(struct Zone *ptrZONE, int zone)
{
  for (int x = 0; x < sizeof(ptrZONE->SET_ZONE); x++) {
    EEPROM.write(eeAddress_startZONES + ((zone - 1)*sizeof(ptrZONE->SET_ZONE)) + x, 0);
  }
}
