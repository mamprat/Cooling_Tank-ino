#include <OneWire.h>
#include <DallasTemperature.h>
#include <DMD2.h>
#include <SPI.h>
#include <fonts/SystemFont5x7.h>

// ======= Sensor DS18B20 =======
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ======= Display =======
SoftDMD dmd(1, 1);

// ======= Konfigurasi =======
const int Standard             = 25;
const int TEMP_MIN             = 2;
const int TEMP_MAX             = 60;
const unsigned long INTERVAL_READ  = 1000;
const unsigned long INTERVAL_RESET = 600000;

// ======= Variabel State =======
unsigned long lastReadTime  = 0;
unsigned long lastResetTime = 0;
int memory                  = 0;
int lastDisplayedTemp       = -999;

// ======= Reset & Buffer =======
void (*resetFunc)(void) = 0;
char tempBuf[6];
char svBuf[6];

void setup() {
  Serial.begin(9600);
  Serial.println("== System Boot ==");

  // 1. DMD pertama
  dmd.begin();
  dmd.setBrightness(130);
  dmd.selectFont(SystemFont5x7);

  dmd.clearScreen();
  dmd.drawString(2,  0, "SV");
  dmd.drawString(19, 0, "PV");
  itoa(Standard, svBuf, 10);
  dmd.drawString(2,  9, svBuf);
  dmd.drawString(19, 9, "--");

  // 2. Init DS18B20
  sensors.begin();
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Sensor ditemukan: ");
  Serial.println(deviceCount);

  if (deviceCount == 0) {
    dmd.clearScreen();
    dmd.drawString(2, 0, "Err");
    dmd.drawString(2, 8, "DS!");
    Serial.println("ERROR: DS18B20 tidak ditemukan!");
    while (1);
  }

  // Tampilan header final
  dmd.clearScreen();
  dmd.drawString(2,  0, "SV");
  dmd.drawString(19, 0, "PV");
  itoa(Standard, svBuf, 10);
  dmd.drawString(2,  9, svBuf);

  Serial.println("== System Ready ==");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastReadTime >= INTERVAL_READ) {
    lastReadTime = currentMillis;

    sensors.requestTemperatures();
    float tempReading = sensors.getTempCByIndex(0);
    int temp = (int)tempReading;

    Serial.print("Temp: ");
    Serial.print(tempReading, 1);
    Serial.println(" C");

    if (tempReading != DEVICE_DISCONNECTED_C && temp > TEMP_MIN && temp < TEMP_MAX) {
      memory = temp;
    } else {
      temp = memory;
      Serial.println("Peringatan: Bacaan invalid, pakai nilai memori.");
    }

    if (temp != lastDisplayedTemp) {
      lastDisplayedTemp = temp;
      dmd.drawFilledBox(19, 9, 31, 15, GRAPHICS_OFF);
      itoa(temp, tempBuf, 10);
      dmd.drawString(19, 9, tempBuf);
      Serial.print("Display PV: ");
      Serial.println(temp);
    }
  }

  if (currentMillis - lastResetTime >= INTERVAL_RESET) {
    Serial.println("Auto Reset...");
    resetFunc();
  }
}
