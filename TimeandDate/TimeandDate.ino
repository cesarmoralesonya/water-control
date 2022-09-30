#include <LiquidCrystal.h>
//initialize the library with the numbers of the interface pins:
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#include <TimerOne.h>
struct Date {
  unsigned  int day;
  unsigned  int month;
  unsigned int year;
} D;
struct Time {
  unsigned  int hour;
  unsigned  int minute;
  unsigned int second;
} T;
static int numZONES;
/*struct Zone {
  float temperature;
  float humidity;
  float tempMAX;
  float humMAX;
  float humMIN;
  struct Time watering;
  } ZONE[4];*/
void setup() {
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  Timer1.initialize(1000000);
  setupTime(&T);
  setupDate(&D);
  Timer1.attachInterrupt(Timer);
  lcd.begin(16, 2);
}
void loop() {
  analogWrite(11, 5);
  lcd.setCursor(0, 0);
  printDate();
  lcd.setCursor(0, 1);
  printTime();
}
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
void printDate(void)//funciona 6/6/2016
{
  if (D.day < 10)lcd.print("0"); lcd.print(D.day); lcd.print("/"); if (D.month < 10)lcd.print("0"); lcd.print(D.month); lcd.print("/"); lcd.print(D.year);
}
void printTime(void)//funciona 6/6/2016
{
  if (T.hour < 10)lcd.print("0"); lcd.print(T.hour); lcd.print(":"); if (T.minute < 10)lcd.print("0"); lcd.print(T.minute);
  lcd.print(":"); if (T.second < 10)lcd.print("0"); lcd.print(T.second);
}
void setupTime(struct Time *ptrT)//funciona 29/5/2016
{
  char TimeforSet[9];
  strcpy(TimeforSet, __TIME__);
  ptrT->hour = 10 * (TimeforSet[0] - 48) + (TimeforSet[1] - 48);
  ptrT->minute = 10 * (TimeforSet[3] - 48) + (TimeforSet[4] - 48);
  ptrT->second = 10 * (TimeforSet[6] - 48) + (TimeforSet[7] - 48);
}
void setupDate(struct Date *ptrD)//funciona 6/6/2016
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


