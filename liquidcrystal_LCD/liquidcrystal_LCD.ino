#include <LiquidCrystal.h>
#define LCD_RSpin 2
#define LCD_Epin 3
#define LCD_D4pin 4
#define LCD_D5pin 5
#define LCD_D6pin 7
#define LCD_D7pin 8
//initialize the library with the numbers of the interface pins:
LiquidCrystal lcd(LCD_RSpin, LCD_Epin, LCD_D4pin, LCD_D5pin, LCD_D6pin, LCD_D7pin);
#define Brigthness 6
void setup() {
  //set up  the LCD's number of columns and rows:
  pinMode(6,OUTPUT);
  lcd.begin(16,2);
  lcd.print("hello world");
  Serial.begin(9600);
}

void loop() {
  static int value_Brigthness=150;
  static int value_Contrast=250;
  analogWrite(Brigthness,value_Brigthness);
  if(Serial.available()>0){
    String cmd;
    Serial.println(cmd);
    value_Contrast=cmd.toInt();
  }
  analogWrite(Contrast,85);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}
