#include <TimerOne.h>
struct Date {
  unsigned int day;
  unsigned  int month;
  unsigned  int year;
} D = {27, 3, 2020};
struct Time {
  unsigned int hour;
  unsigned  int minute;
  unsigned  int second;
} T = {01, 59, 59};
boolean summer_time;
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(TimerIsr);
  //pinMode(2,INPUT);
}
void loop() {
  summer_time = true; //digitalRead(2);
  printTimeandDate();
}
void TimerIsr()
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
  Date();
}
void Date()
{
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
  if (summer_time == true)summer_opening_times();
}
void summer_opening_times(void)
{
  //each 27 march the clocks go forward
  //2:00 A.M. -> 3:00 A.M.
  if (D.day == 27 && D.month == 3 && T.hour == 2) T.hour = 3;
  //each 30 october the clocks go retard
  //3:00 A.M. -> 2:00 A.M.
  if (D.day == 30 && D.month == 10 && T.hour == 3) T.hour = 2;
}
void printTimeandDate(void)
{
  Serial.print(D.day); Serial.print("/"); Serial.print(D.month); Serial.print("/"); Serial.print(D.year); Serial.print("\t");
  if (T.hour < 10)Serial.print("0"); Serial.print(T.hour); Serial.print(":"); if (T.minute < 10)Serial.print("0"); Serial.print(T.minute);
  Serial.print(":"); if (T.second < 10)Serial.print("0"); Serial.println(T.second);
}
