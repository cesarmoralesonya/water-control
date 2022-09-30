////////////////////////////TimeandDate///////////////////////////////
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
#define LCD_RSpin 7
#define LCD_Epin 6
#define LCD_D4pin 5
#define LCD_D5pin 4
#define LCD_D6pin 3
#define LCD_D7pin 2
//initialize the library with the numbers of the interface pins:
LiquidCrystal lcd(LCD_RSpin, LCD_Epin, LCD_D4pin, LCD_D5pin, LCD_D6pin, LCD_D7pin);
#define pin_Brightness 11
#define Bright_MAX 255
#define Bright_MED 150
#define Bright_MIN 50
int Brightness;
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
} ZONE[6];
///////////////////flags for Sell comand //////////////////////////
bool FlagNZONESset = true;
bool FlagTHset = true;
bool FlagWATERING_HOURSset = true;
///////////////////flags for WORKING //////////////////////////
bool FlagON = false;
bool Flagprintset = true;
/////////////////////////  EEPROM  ///////////////////////////////
#include <EEPROM.h>
//EEPROM_mannager:
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
/////////////////////////  BUTTONS  ///////////////////////////////
#define buttonRESET 13
////////////////////////////////////////////////////SETUP///////////////////////////////////////////////////////////
////////////////////////////////////////////////////SETUP///////////////////////////////////////////////////////////
void setup() {
  ////////////////TimeandDate///////////////////
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(Timer);
  //set up clock and calendar:
  setupTime(&T);
  setupDate(&D);
  ////////////for_liquidcrystal_LCD/////////////
  //set up  the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //brightness lcd
  pinMode(pin_Brightness, OUTPUT);
  pinMode(buttonRESET, INPUT);
  ////////////driverPCF5748 I2C////////////////
  Wire.begin();
  Wire.beginTransmission(0x38);
  Wire.write(code);
  Wire.endTransmission();
  //////////////Sell_comands//////////////////
  Serial.begin(9600);
  /////////////READ EEPROM///////////////////
  if (eeBussy(eeAddress_numZONES, eeAddress_FlagWATERING_HOURSset) == true) {//work
    numZONES = READee_int(eeAddress_numZONES);
    READee_FLAGS();
  }
  if (EEPROM.read(eeAddress_FlagON) != 0)FlagON = EEPROM.read(eeAddress_FlagON);
  if (eeBussy(eeAddress_startZONES, TOP_Address) == true)READee_ZONES(ZONE);//work
}
///////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////////
///////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////////
void loop() {
  if (FlagON == false)Brightness = 0;
  else {
    if (T.hour >= 7 && T.hour < 12)Brightness = Bright_MED;
    if (T.hour >= 12 && T.hour < 20)Brightness = Bright_MAX;
    if (T.hour >= 20 && (T.hour < 23 && T.minute <= 59))Brightness = Bright_MIN;
    if (T.hour >= 0 && T.hour < 7)Brightness = Bright_MIN;
  }
  analogWrite(pin_Brightness, Brightness);
  //////////////////////////////////////////SELL_COMANDS//////////////////////////////////////
  if (Serial.available() > 0) {
    String cmd = Serial.readString();
    Serial.println(cmd);
    //SET_NUM_ZONES
    if (cmd.substring(0, 13) == "SET_NUM_ZONES") {
      FlagNZONESset = true;
      FlagTHset = false;
      FlagWATERING_HOURSset = false;
      Flagprintset = true;
      numZONES = 0;
      WRITEee_int(eeAddress_numZONES, numZONES);
      WRITE_FLAGS_SET();
      lcd.clear();
    }
    //SET_TEMP_HUM
    if (cmd.substring(0, 12) == "SET_TEMP_HUM") {
      FlagNZONESset = false;
      FlagTHset = true;
      FlagWATERING_HOURSset = false;
      Flagprintset = true;
      WRITE_FLAGS_SET();
      lcd.clear();
    }
    //SET_WATERING_HOURS
    if (cmd.substring(0, 18) == "SET_WATERING_HOURS") {
      FlagNZONESset = false;
      FlagTHset = false;
      FlagWATERING_HOURSset = true;
      Flagprintset = true;
      WRITE_FLAGS_SET();
      lcd.clear();
    }
    //PRINT_SETTINGS
    if (cmd.substring(0, 14) == "PRINT_SETTINGS") {
      print_settings(ZONE);
    }
    //ON
    if (cmd.substring(0, 2) == "ON") {
      FlagON = true;
      //EEPROM.write(eeAddress_FlagON, FlagON);
    }
    //OFF
    if (cmd.substring(0, 3) == "OFF") {
      FlagON = false;
      //EEPROM.write(eeAddress_FlagON, FlagON);
      lcd.clear();
    }
    if (cmd.substring(0, 5) == "RESET") {
      eeClean(0, TOP_Address);
      lcd.clear();
      FlagNZONESset = true;
      FlagTHset = true;
      FlagWATERING_HOURSset = true;
      WRITE_FLAGS_SET();
    }
  }
  if (FlagON == true) {
    //////////////////////////////NUMBER OF ZONES////////////////
    if (FlagNZONESset == true) {
      lcd.setCursor(0, 0);
      lcd.print("SETTING:");
      lcd.setCursor(0, 1);
      lcd.print("NUMBER OF ZONES");
      interfaceSet_numZONES();
    }
    /////////////////////////TEMPERATURE AND HUMIDITY//////////////////////
    if (FlagNZONESset == false && FlagTHset == true) {
      lcd.setCursor(0, 0);
      lcd.print("SETTING:");
      lcd.setCursor(0, 1);
      lcd.print("HUMIDY AND TEMPE");
      interfaceSet_TEMP_HUM(ZONE);
    }
    ////////////////////////////WATERING HOURS///////////////////////////
    if (FlagNZONESset == false && FlagTHset == false && FlagWATERING_HOURSset == true) {
      lcd.setCursor(0, 0);
      lcd.print("SETTING:");
      lcd.setCursor(0, 1);
      lcd.print("WATERING HOURS");
      interfaceSet_WATERING_HOURS(ZONE);
    }
  }
  //////////////////////////////////////////SELL_COMANDS////////////////////////////////////////////
  if (FlagNZONESset == false && FlagTHset == false && FlagWATERING_HOURSset == false && FlagON == true) {
    static int RESET;
    if (digitalRead(buttonRESET) == HIGH) {
      RESET++;
      if (RESET > 100) {
        eeClean(0, TOP_Address);
        FlagNZONESset = true;
        FlagTHset = true;
        FlagWATERING_HOURSset = true;
        WRITE_FLAGS_SET();
        lcd.clear();
      }
    } else RESET = 0;
    ///////////////////////////READING TEMPERATURE AND HIMIDITY BY SENSORS///////////////////////////
    for (int y = 0; y < numZONES; y++) {
      ZONE[y].temperature = SENSOR(temperature, allzones);
      if (y <= 1)ZONE[y].humidity = SENSOR(humidity, zone1y2);
      if (y > 1)ZONE[y].humidity = SENSOR(humidity, zone3y4);
    }
    ///////////////////////////READING TEMPERATURE AND HIMIDITY BY SENSORS///////////////////////////
    ///////////////////////////////////////////////////representation on LCD////////////////////////////////////////////////////
    printTEMP_AND_HUM(ZONE);
    ///////////////////////////////////////////////////representation on LCD////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////WATERING/////////////////////////////////////////////////////////
    for (int x = 0; x < numZONES; x++) {
      ///////////////////////////////////////////////check hours////////////////////////////////////////
      if (T.hour < ZONE[x].SET_ZONE.wateringOFF.hour) {
        if ((T.hour >= ZONE[x].SET_ZONE.wateringON.hour && T.hour < ZONE[x].SET_ZONE.wateringOFF.hour) ||
            (T.hour == ZONE[x].SET_ZONE.wateringON.hour && T.minute == ZONE[x].SET_ZONE.wateringON.minute))ZONE[x].FLAGwatering = true;
      }
      if (T.hour == ZONE[x].SET_ZONE.wateringOFF.hour) {
        if (T.minute < ZONE[x].SET_ZONE.wateringOFF.minute)ZONE[x].FLAGwatering = true;
        else ZONE[x].FLAGwatering = false;
      }
      if (T.hour > ZONE[x].SET_ZONE.wateringOFF.hour) ZONE[x].FLAGwatering = false;
      /////////////////////////////////////////////////////////////////////////////////////////////////////////
      if ((ZONE[x].FLAGwatering == true) && (ZONE[x].humidity < ZONE[x].SET_ZONE.humMAX)
          && (ZONE[x].temperature < ZONE[x].SET_ZONE.tempMAX)) {
        PCF8574WRITE(x, HIGH );
        digitalWrite(x + 4, HIGH );
      } else {
        PCF8574WRITE(x, LOW );
        digitalWrite(x + 4, LOW);
      }
      ///// auto-watering
      if (ZONE[x].humidity < ZONE[x].SET_ZONE.humMIN) {
        unsigned long Time = millis();
        ///detecta el paso por el humbral de humedad.
        if (ZONE[x].FLAGshoot_time == true) {
          ZONE[x].Last_time = Time;
          ZONE[x].FLAGshoot_time = false;
        }
        if ((Time - ZONE[x].Last_time) > ((ZONE[x].SET_ZONE.Auto) * 60000)) {
          PCF8574WRITE(x, HIGH );
          digitalWrite(x + 4, HIGH );
        }
      } else ZONE[x].FLAGshoot_time = true;
    }
    //////////////////////////////////////////////////////////////WATERING/////////////////////////////////////////////////////////
  }
}
//////////////////////////////////////////////////////////FUNTIONS///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////FUNTIONS//////////////////////////////////////////////////////////

/////////Time and Date////////////
void Timer()
{
  T.second++;
  if (T.second == 60) {
    T.minute++;
    T.second = 0;
  } if (T.minute == 60) {
    T.hour++;
    T.minute = 0;
    if (T.hour == 24)T.hour = 0;
  }
  ////////////////////////Date///////////////////////
  //set leap-year:funciona
  int february_days;
  if (D.year % 4 == 0)february_days = 29;
  else february_days = 28;
  //set days of the month:funciona
  static int days, days_of_parMonth, days_of_imparMonth;
  if (D.month <= 7)days_of_parMonth = 30, days_of_imparMonth = 31;
  else days_of_parMonth = 31, days_of_imparMonth = 30;
  //par or impar month:funciona
  if (D.month % 2 == 0) {
    days = days_of_parMonth;
    if (D.month == 2)days = february_days;
  } else days = days_of_imparMonth;
  //calendar: funciona
  ///////////
  if (T.hour == 0 && T.minute == 0 && T.second == 0)D.day++;
  if (D.day > days && T.hour == 0 && T.minute == 0 && T.second == 0) {
    D.month++;
    if (D.day > days)D.day = 1;
  }
  if (D.month >= 12 && D.day == 1 && T.hour == 0 && T.minute == 0 && T.second == 0) {
    D.year++;
    if (D.month >= 12)D.month = 1;
  }
  ///////////////summer_time/////////////////////////
  //each 27 march the clocks go forward
  //2:00 A.M. -> 3:00 A.M.
  if (D.day == 27 && D.month == 3 && T.hour == 2) T.hour = 3;
  //each 30 october the clocks go retard
  //3:00 A.M. -> 2:00 A.M.
  if (D.day == 30 && D.month == 10 && T.hour == 3) T.hour = 2;
}
void setupTime(struct Time * ptrT) //funciona 10/5/2016
{
  char TimeforSet[9];
  strcpy(TimeforSet, __TIME__);
  ptrT->hour = 10 * (TimeforSet[0] - '0') + (TimeforSet[1] - 48);
  ptrT->minute = 10 * (TimeforSet[3] - 48) + (TimeforSet[4] - 48);
  ptrT->second = 10 * (TimeforSet[6] - 48) + (TimeforSet[7] - 48);
}
void setupDate(struct Date * ptrD) //funciona 10/6/2016
{
  char DateforSet[12];
  strcpy(DateforSet, __DATE__);
  //Set year:funciona
  ptrD->year = (DateforSet[10] - 48) + 10 * (DateforSet[9] - 48) + 100 * (DateforSet[8] - 48) + 1000 * (DateforSet[7] - 48);
  //Set month:funciona
  String Month;
  Month = DateforSet;
  Month = Month.substring(0, 3);
  if (Month == "Jan")ptrD->month = 1; if (Month == "Feb")ptrD->month = 2; if (Month == "Mar")ptrD->month = 3; if (Month == "Apr")ptrD->month = 4;
  if (Month == "May")ptrD->month = 5; if (Month == "Jun")ptrD->month = 6; if (Month == "Jul")ptrD->month = 7; if (Month == "Aug")ptrD->month = 8;
  if (Month == "Sep")ptrD->month = 9; if (Month == "Oct")ptrD->month = 10; if (Month == "Nov")ptrD->month = 11; if (Month == "Dec")ptrD->month = 12;
  //Set day:funciona
  if (DateforSet[4] != ' ')ptrD->day = (DateforSet[5] - 48) + 10 * (DateforSet[4] - 48);
  else ptrD->day = (DateforSet[5] - 48);
}
///////////////////////////////////////////////////////////////Sell comand interfaces///////////////////////////////////////////////////////////////////
void interfaceSet_numZONES(void)
{
  if (Flagprintset == true && (numZONES < 1 || numZONES > 6)) {
    Serial.println("\nnumber  of watering zones?\n");
    Flagprintset = false;
  }
  while (Serial.available() > 0)
  {
    String cmd = Serial.readString();
    Serial.println(cmd);
    numZONES = cmd.toInt();
    if (numZONES > 0 && numZONES < 7) {
      Serial.println("\nnumber of zones configured");
      Flagprintset = true;
      FlagNZONESset = false;
      lcd.clear();
      ////////////////////Save numZONES into EEPROM/////////////////
      WRITEee_int(eeAddress_numZONES, numZONES);
      /////////////////Save FlagNZONESset into EEPROM///////////////
      EEPROM.write(eeAddress_FlagNZONESset, FlagNZONESset);
    } else {
      Serial.println("\nincorrect value\n");
      Flagprintset = true;
    }
  }
}
void interfaceSet_TEMP_HUM(struct Zone * ptrZONE)
{
  String cmd;
  //////////////////configuracion Temperature & Humidity////////////////////////
  if (Flagprintset == true && FlagTHset == true && (numZONES > 0 && numZONES < 7)) {
    Serial.println("\n\t\t\t\tSetting Temperature & Humidity\n");
    Flagprintset = false;
  }
  if (FlagTHset == true) {
    while (Serial.available() > 0)
    {
      cmd = Serial.readString();
      Serial.print("\n  comand: "); Serial.println(cmd);
      if (cmd.substring(0, 7) == "END_SET") {
        /////DA TODOS LOS VALORES CONFIGURADOS
        for (int x = 0; x < numZONES; x++) {
          Serial.println("\n");
          Serial.print("ZONE "); Serial.println(x + 1);
          Serial.print("temperature MAX: "); Serial.print((ptrZONE + x)->SET_ZONE.tempMAX, 0); Serial.println(" 'C");
          Serial.print("humidity MAX: "); Serial.print((ptrZONE + x)->SET_ZONE.humMAX, 0); Serial.println(" %");
          Serial.print("humidity MIN: "); Serial.print((ptrZONE + x)->SET_ZONE.humMIN, 0); Serial.println(" %");
        }
        lcd.clear();
        Flagprintset = true;
        FlagTHset = false;
        /////////////////Save FlagTHset into EEPROM///////////////
        EEPROM.write(eeAddress_FlagTHset, FlagTHset);
      }
      else {
        //PRUEBAS: ZONE_ALL_Tmax=45/Hmax=80/Hmin=20
        if (cmd.substring(5, 8) == "ALL") {
          for (int x = 0; x < numZONES; x++) {
            (ptrZONE + x)->SET_ZONE.tempMAX = cmd.substring(14, 16).toInt();
            (ptrZONE + x)->SET_ZONE.humMAX = cmd.substring(22, 24).toInt();
            (ptrZONE + x)->SET_ZONE.humMIN = cmd.substring(30, 32).toInt();
          }
        } else {
          //ZONE_n_Tmax=XX/Hmax=XX/Hmin=XX
          //PRUEBAS: ZONE_1_Tmax=45/Hmax=80/Hmin=20
          //ZONE_1_Tmax=35/Hmax=60/Hmin=20
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.tempMAX = cmd.substring(12, 14).toInt();
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.humMAX = cmd.substring(20, 22).toInt();
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.humMIN = cmd.substring(28, 30).toInt();
        }
      }
    }
  }
}
void interfaceSet_WATERING_HOURS(struct Zone * ptrZONE)
{
  //////////////////mensaje configuracion horas de riego////////////////////////////
  if (Flagprintset == true) {
    Serial.println("\n\t\t\t\t\tSetting Watering hours:\n");
    Flagprintset = false;
  }
  while (Serial.available() > 0) {
    String cmd = Serial.readString();
    Serial.print("\n  comand: "); Serial.println(cmd);
    if (cmd.substring(0, 7) == "END_SET") {
      for (int x = 0; x < numZONES; x++) {
        ////print all watering ON
        Serial.print("\nthe watering ZONE  "); Serial.print(x + 1); Serial.print(" ON is ");
        Serial.print(ptrZONE->SET_ZONE.wateringON.hour); Serial.print(":");
        if (ptrZONE->SET_ZONE.wateringON.minute > 10)Serial.println(ptrZONE->SET_ZONE.wateringON.minute);
        else {
          Serial.print("0");
          Serial.println(ptrZONE->SET_ZONE.wateringON.minute);
        }
        ////print all watering OFF
        Serial.print("\nthe watering ZONE  "); Serial.print(x + 1); Serial.print(" OFF is ");
        Serial.print(ptrZONE->SET_ZONE.wateringOFF.hour); Serial.print(":");
        if (ptrZONE->SET_ZONE.wateringOFF.minute > 10)Serial.println(ptrZONE->SET_ZONE.wateringOFF.minute);
        else {
          Serial.print("0");
          Serial.println(ptrZONE->SET_ZONE.wateringOFF.minute);
        }
        ////print all AUTO-watering
        Serial.print("\nthe auto-watering ZONE  "); Serial.print(x + 1); Serial.print(" is ");
        if (ptrZONE->SET_ZONE.Auto == 0)ptrZONE->SET_ZONE.Auto = 1; //default auto-watering time put to 1 minute
        Serial.print(ptrZONE->SET_ZONE.Auto, 0); Serial.println(" minutes");
        ptrZONE++;
      }
      lcd.clear();
      Flagprintset = true;
      FlagWATERING_HOURSset = false;
      /////////////////Save FlagWATERING_HOURSset into EEPROM///////////////
      EEPROM.write(eeAddress_FlagWATERING_HOURSset, FlagWATERING_HOURSset);
      //EEPROM.write(eeAddress_FlagON, FlagON);
      //////////////////////Save SETTINGS OF ZONES/////////////////////////
      WRITEee_ZONES(ZONE);
    }
    else {
      //ZONE_ALL_ON=hh:mm/OFF=hh:mm/AUTO=mm
      //ZONE_ALL_ON=15:00/OFF=23:30/AUTO=2
      if (cmd.substring(5, 8) == "ALL") {
        for (int x = 0; x < numZONES; x++) {
          (ptrZONE + x)->SET_ZONE.wateringON.hour = cmd.substring(12, 14).toInt();
          (ptrZONE + x)->SET_ZONE.wateringON.minute = cmd.substring(15, 17).toInt();
          (ptrZONE + x)->SET_ZONE.wateringOFF.hour = cmd.substring(22, 24).toInt();
          (ptrZONE + x)->SET_ZONE.wateringOFF.minute = cmd.substring(25, 27).toInt();
          (ptrZONE + x)->SET_ZONE.Auto = cmd.substring(33, 35).toInt();
        }

      } else {
        //ZONE_n_ON:hh:mm
        if (cmd.substring(7, 9) == "ON") {
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.wateringON.hour = cmd.substring(10, 12).toInt();
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.wateringON.minute = cmd.substring(13, 15).toInt();
        }
        //ZONE_n_OFF:hh:mm
        if (cmd.substring(7, 10) == "OFF") {
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.wateringOFF.hour = cmd.substring(11, 13).toInt();
          (ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.wateringOFF.minute = cmd.substring(14, 16).toInt();
        }
        //ZONE_n_AUTO=mm
        if (cmd.substring(7, 11) == "AUTO")(ptrZONE + cmd.substring(5, 6).toInt() - 1)->SET_ZONE.Auto = cmd.substring(12, 14).toInt();
      }
    }
  }
}
////////////////////////////////////////////////////////////////////Sensors/////////////////////////////////////////////////////////////////
void print_settings(struct Zone * ptrZONE)
{
  ////////////////////print temp and hum///////////////////
  for (int x = 0; x < numZONES; x++) {
    Serial.println("\n");
    Serial.print("ZONE "); Serial.println(x + 1);
    Serial.print("temperature MAX: "); Serial.print((ptrZONE + x)->SET_ZONE.tempMAX, 0); Serial.println(" 'C");
    Serial.print("humidity MAX: "); Serial.print((ptrZONE + x)->SET_ZONE.humMAX, 0); Serial.println(" %");
    Serial.print("humidity MIN: "); Serial.print((ptrZONE + x)->SET_ZONE.humMIN, 0); Serial.println(" %");
  }
  ////////////////////print watering hours///////////////////
  for (int x = 0; x < numZONES; x++) {
    ////print all watering ON
    Serial.print("\nthe watering ZONE  "); Serial.print(x + 1); Serial.print(" ON is ");
    Serial.print(ptrZONE->SET_ZONE.wateringON.hour); Serial.print(":");
    if (ptrZONE->SET_ZONE.wateringON.minute > 10)Serial.println(ptrZONE->SET_ZONE.wateringON.minute);
    else {
      Serial.print("0");
      Serial.println(ptrZONE->SET_ZONE.wateringON.minute);
    }
    ////print all watering OFF
    Serial.print("\nthe watering ZONE  "); Serial.print(x + 1); Serial.print(" OFF is ");
    Serial.print(ptrZONE->SET_ZONE.wateringOFF.hour); Serial.print(":");
    if (ptrZONE->SET_ZONE.wateringOFF.minute > 10)Serial.println(ptrZONE->SET_ZONE.wateringOFF.minute);
    else {
      Serial.print("0");
      Serial.println(ptrZONE->SET_ZONE.wateringOFF.minute);
    }
    ////print all AUTO-watering
    Serial.print("\nthe auto-watering ZONE  "); Serial.print(x + 1); Serial.print(" is ");
    if (ptrZONE->SET_ZONE.Auto == 0)ptrZONE->SET_ZONE.Auto = 1; //default auto-watering time put to 1 minute
    Serial.print(ptrZONE->SET_ZONE.Auto, 0); Serial.println(" minutes");
    ptrZONE++;
  }
}

////////////////////////////////////////////////////////////////////Sensors/////////////////////////////////////////////////////////////////
float SENSOR(int sensor, int zone)
{
  if (sensor == temperature && zone == allzones) {
    float temp;
    static float last_temp;
    float temp_filter;
    ////LM35 conectado:
    //temp = (analogRead(0) * 5.0 / 1024.0) * 100.0;
    ////potenciometro conectado:
    temp = (analogRead(0) / 1024.0) * 50.0;
    temp_filter = exponential_filter(temp, &last_temp, 0.5);
    last_temp = temp;
    return temp_filter;
  }
  if (sensor == humidity && zone == zone1y2) {
    float hum;
    static float last_hum;
    float hum_filter;
    hum = (analogRead(1) / 1024.0) * 90.0 + 5.0;
    hum_filter = exponential_filter(hum, &last_hum, 0.5);
    last_hum = hum;
    return hum_filter;
  }
  if (sensor == humidity && zone == zone3y4) {
    float hum;
    static float last_hum;
    float hum_filter;
    hum = (analogRead(2) / 1024.0) * 90.0 + 5.0;
    hum_filter = exponential_filter(hum, &last_hum, 0.5);
    last_hum = hum;
    return hum_filter;
  }
}
////////exponential_filter(valor actual,valor anterior,facor de correción);
float exponential_filter(float Xk, float * Yk_1, float a)
{
  float Yk = *Yk_1 + a * (Xk - *Yk_1);
  *Yk_1 = Yk;
  return Yk;
}
////////////////////////////////////////////////liquidcrystal_LCD representation///////////////////////////////////////////////
void printTEMP_AND_HUM(struct Zone ZONE[4])
{
  ///////////////////TEMPERATURE////////////////////
  lcd.setCursor(3, 0);
  lcd.print("TEMPE:");
  if (ZONE[0].temperature > 10)lcd.setCursor(9, 0);
  else {
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(10, 0);
  }
  lcd.print(ZONE[0].temperature, 0);
  lcd.setCursor(11, 0);
  lcd.print("'C");
  ////////////////////HUMIDITY///////////////////
  lcd.setCursor(0, 1);
  lcd.print("HUMID:");
  ////////////HUMIDITY ZONE 1 AND 2///////////////
  if (ZONE[0].humidity > 10)lcd.setCursor(7, 1);
  else {
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
  }
  lcd.print(ZONE[0].humidity, 0);
  lcd.setCursor(9, 1);
  lcd.print("%");
  lcd.setCursor(10, 1);
  lcd.print(" | ");
  ////////////HUMIDITY ZONE 3 AND 4///////////////
  if (numZONES > 2) {
    if (ZONE[2].humidity > 10)lcd.setCursor(13, 1);
    else {
      lcd.setCursor(13, 1);
      lcd.print(" ");
      lcd.setCursor(14, 1);
    }
    lcd.print(ZONE[2].humidity, 0);
    lcd.setCursor(15, 1);
    lcd.print("%");
  } else {
    lcd.setCursor(13, 1);
    lcd.print("OFF");
  }
}
/*void printDate(void)//funciona 6/6/2016
  {
  if (D.day < 10)lcd.print("0"); lcd.print(D.day); lcd.print("/"); if (D.month < 10)lcd.print("0"); lcd.print(D.month); lcd.print("/"); lcd.print(D.year);
  }
  void printTime(void)//funciona 6/6/2016
  {
  if (T.hour < 10)lcd.print("0"); lcd.print(T.hour); lcd.print(":"); if (T.minute < 10)lcd.print("0"); lcd.print(T.minute);
  lcd.print(":"); if (T.second < 10)lcd.print("0"); lcd.print(T.second);
  }*/
/////////////////////////////////////////DriverPCF8574 I2C/////////////////////////////////
void PCF8574WRITE(unsigned int Gate, boolean ORDER )
{
  if (ORDER == LOW) {
    code = code | (0x01 << Gate);
    Wire.beginTransmission(0x38);
    Wire.write(code);
    Wire.endTransmission();
  }
  if (ORDER == HIGH) {
    code = code & (0xff - (0x01 << Gate));
    Wire.beginTransmission(0x38);
    Wire.write(code);
    Wire.endTransmission();
  }
}
boolean PCF8574READ(unsigned int Gate)
{
  if (bitRead(code, Gate) == 0)return  HIGH;
  if (bitRead(code, Gate) == 1)return LOW;
}
/////////////////////////////////////////EEPROM_WRITE/////////////////////////////////////
void WRITEee_int(int adress_start, int AuxVAL) //guardar int en la eeprom
{
  byte H = highByte(AuxVAL);
  byte L = lowByte(AuxVAL);
  EEPROM.write(adress_start, L);
  EEPROM.write(adress_start + 1, H);
}
void WRITE_FLAGS_SET(void) {
  EEPROM.write(eeAddress_FlagNZONESset, FlagNZONESset);
  EEPROM.write(eeAddress_FlagTHset, FlagTHset);
  EEPROM.write(eeAddress_FlagWATERING_HOURSset, FlagWATERING_HOURSset);
}
void WRITEee_ZONES(struct Zone * ptrZONE )//work 16:08 funcion guarda estructura zonas
{
  int sizeAddress = sizeof(ptrZONE->SET_ZONE);
  for (int x = 0; x < numZONES; x++) {
    EEPROM.put(eeAddress_startZONES + (x * sizeAddress), ((ptrZONE + x)->SET_ZONE) );
  }
  Serial.println("ZONES SAVED");
}
/////////////////////////////////////////EEPROM_READ/////////////////////////////////////
int READee_int(int address_start)//work: 14:59 funcion guarda variables de tipo int
{
  int AuxVAL;
  int L = EEPROM.read(address_start);
  int H = EEPROM.read(address_start + 1);
  H = H << 8;
  AuxVAL = H | L;
  return AuxVAL;
}
void READee_FLAGS(void)//work: 15:34
{
  FlagNZONESset = EEPROM.read(eeAddress_FlagNZONESset);
  FlagTHset = EEPROM.read(eeAddress_FlagTHset);
  FlagWATERING_HOURSset = EEPROM.read(eeAddress_FlagWATERING_HOURSset);
}
void READee_ZONES(struct Zone * ptrZONE )//work 16:08
{
  int sizeAddress = sizeof(ptrZONE->SET_ZONE);
  for (int x = 0; x < numZONES; x++) {
    EEPROM.get(eeAddress_startZONES + (x * sizeAddress), ((ptrZONE + x)->SET_ZONE) );
  }
}
/////////////////////////////////////////EEPROM_MANNAGER/////////////////////////////////////
bool eeBussy(int Start, int Finish) //work 14:00
{
  int EEPROM_free = 0;
  for (int i = Start; i < Finish ; i++)
  {
    if (EEPROM.read(i) == 0)EEPROM_free++;
  }
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
void eeClean_ZONE(struct Zone * ptrZONE, int zone) //work 17:15
{
  for (int x = 0; x < sizeof(ptrZONE->SET_ZONE); x++) {
    EEPROM.write(eeAddress_startZONES + ((zone - 1)*sizeof(ptrZONE->SET_ZONE)) + x, 0);
  }
}
