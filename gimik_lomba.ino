
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 


void setup() {
  // put your setup code here, to run once:
  lcd.begin(20,4);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.print("O3: 15.5 ppm");
   lcd.setCursor(0,1);
    lcd.print("Hmdt: 61.5 %");
  lcd.setCursor(0,2);
    lcd.print("Temp: 31oC");
  lcd.setCursor(0,3);
  lcd.print("Status: Normal");  
}
