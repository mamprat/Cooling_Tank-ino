// //======= sensor ds18b20 =======
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// //======= display dot matrix =======
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>

SoftDMD dmd(1, 1);
DMD_TextBox box(dmd, 1, 1, 32, 16);

// //======= lamp =======
const int Trigger_Lamp = 5;
const int Trigger_Alarm = 4;
int temp;
int Standard;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  dmd.begin();
  delay(100);

  pinMode(Trigger_Lamp, OUTPUT);
  pinMode(Trigger_Alarm, OUTPUT);

  dmd.setBrightness(150);
  dmd.selectFont(SystemFont5x7);
}

void loop() {
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  if (temp > -127.00 && temp < 85.00) {
    Serial.print(temp);
    delay(1000);
    dmd.drawString(2, 0, String("St"));
    dmd.drawString(19, 0, String("Pv"));
    dmd.drawString(2, 8, String("25"));
    dmd.drawString(19, 8, String(temp));

    delay(10000);

  } else {
    // int memory = random(14.6, 15.3);
    int memory = temp;
    Serial.print(memory);
    delay(1000);
    dmd.drawString(19, 8, String(memory));
    delay(50);
  }

  Standard = 25;  //20
  if (temp > Standard) {
    digitalWrite(Trigger_Lamp, LOW);
    digitalWrite(Trigger_Alarm, LOW);
  } else {
    digitalWrite(Trigger_Lamp, HIGH);
    digitalWrite(Trigger_Alarm, HIGH);
    dmd.drawString(1, 1, String("error"));
  }
}
