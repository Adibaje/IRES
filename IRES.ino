/*******************************************************************************
 * Sample the ozone concentration every 60 seconds
 * 
 * Example code base on high concentration sensor (metal)
 * and load resistance of 1MOhms
 * 
 * Schematics and details available on https://github.com/ostaquet/Arduino-MQ131-driver
 ******************************************************************************
 * MIT License
 *
 * Copyright (c) 2018 Olivier Staquet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/

#include <MQ131.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"


#define DHTPIN 3  //Connected to PIN 3  
#define DHTTYPE DHT22   //Connected DHT

//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

DHT dht(DHTPIN, DHTTYPE);

int relay1 = 4;
int relay2 = 5;
float ppmValue;
float dummy = 1;
int suhu;
int buzzer = 6;

void setupRelay(){
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1,LOW);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2,LOW);
}



void setupLCD(){
  pinMode(buzzer, OUTPUT);
  lcd.begin(20,4);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();//To Power ON the back light
  //lcd.backlight();// To Power OFF the back light
 
  lcd.setCursor(2,2);
  lcd.print("Welcome To IRES!");
  digitalWrite(buzzer,HIGH);
  delay(3000);
  digitalWrite(buzzer,LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calibrating....");
  
  lcd.setCursor(0,1);
  lcd.print("R0: ");
  lcd.print(MQ131.getR0());
  lcd.print(" Ohms");
  lcd.setCursor(0,2);
  lcd.print("T2H: ");
  lcd.print(MQ131.getTimeToRead());
  lcd.print(" s");
  
}

void setupTemp(){ //To initiate use of DHT22 Temp.Sensor
  Serial.print("Temp. Sensor Starting!");
  dht.begin();
}

void callRelay(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float hic = dht.computeHeatIndex(t, h, false);
  

  Serial.print("parameter suhu: ");
  Serial.println(suhu);
  
  ppmValue = MQ131.getO3(PPM);
  Serial.print("nilai PPM:" );
  Serial.println(ppmValue);

 Serial.print("Status: ");
  lcd.setCursor(0,3);
  lcd.print("Status: ");
  
  if ((ppmValue <= dummy) && (suhu <= 0)){
    digitalWrite(relay1,LOW);
   Serial.print("Normal");
    lcd.print("Normal");
  } else {
    digitalWrite(relay1,HIGH);
    Serial.print("Abnormal");
    lcd.print("Abnormal"); 
  }

  if (hic <= 34.0){
    digitalWrite(relay2,LOW);
    suhu = 0;
  } else {
    digitalWrite(relay2,HIGH);
    suhu = 1;
  }
  
}

void tempSensor(){ //To occupy DHT22 Temp Sensor
    //delay(2000); //To print in every 2s
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    //float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    //float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    //Serial.print(F("%  Temperature: "));
    //Serial.print(t);
    //Serial.print(F("oC "));
    //Serial.print(f);
    Serial.print(F("%  Temperature: "));
    Serial.print(hic);
    Serial.print(F("oC "));
    //Serial.print(hif);
    //Serial.println(F("°F"));

    lcd.setCursor(0,1);
    lcd.print("Hmdt: ");
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor(0,2);
    lcd.print("Temp: ");
    lcd.print(hic);
    lcd.print("oC");
}

void setupOzonSensor(){
  // Init the sensor
  // - Heater control on pin 2
  // - Sensor analog read on pin A0
  // - Model LOW_CONCENTRATION
  // - Load resistance RL of 1MOhms (1000000 Ohms)
  MQ131.begin(2,A0, LOW_CONCENTRATION, 1000000); 
  //Serial.println("Calibration parameters");
}

void ozonSensor(){
  Serial.println("Sampling...");
  MQ131.sample();
  MQ131.setEnv(25, 75);
  Serial.print("Concentration O3 : ");
  Serial.print(MQ131.getO3(PPM));
  Serial.println(" ppm");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("O3: ");
  lcd.print(MQ131.getO3(PPM));
  lcd.print(" ppm");  
}

void setup() {
  Serial.begin(115200);
  setupOzonSensor(); 
  //MQ131.setR0(1917.22);
  //MQ131.setTimeToRead(80);
  setupRelay();
  setupTemp();
  setupLCD(); 
  suhu = 0;
}

void loop() {
  //ozon sensor sampling....
  ozonSensor();

  //temperature sampling
  tempSensor();

  //Main function
  callRelay();
  
}
