#include <Adafruit_MLX90614.h>
#include <DMD2.h>
#include <SPI.h>
#include <fonts/SystemFont5x7.h>
#include <Wire.h>

// ======= Sensor & Display =======
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SoftDMD dmd(1, 1);

// ======= Pin =======
const int Trigger_Lamp  = 5;
const int Trigger_Alarm = 4;

// ======= Konfigurasi =======
const int Standard              = 25;
const int TEMP_MIN              = 2;
const int TEMP_MAX              = 60;
const unsigned long INTERVAL_READ  = 1000;
const unsigned long INTERVAL_RESET = 600000;

// ======= Variabel State =======
unsigned long lastReadTime    = 0;
unsigned long lastResetTime   = 0;
int memory                    = 0;
int lastDisplayedTemp         = -999;
bool sensorOK                 = false;

// ======= Reset & Buffer =======
void (*resetFunc)(void) = 0;
char tempBuf[6];
char svBuf[6];

// ======================================================
void setup() {
  Serial.begin(9600);
  Serial.println("== System Boot ==");

  pinMode(Trigger_Lamp,  OUTPUT);
  pinMode(Trigger_Alarm, OUTPUT);
  digitalWrite(Trigger_Lamp,  HIGH); // Relay OFF
  digitalWrite(Trigger_Alarm, HIGH); // Relay OFF

  // ── 1. DMD PERTAMA, sebelum Wire apapun ──
  dmd.begin();
  dmd.setBrightness(130);
  dmd.selectFont(SystemFont5x7);

  // Tampilan awal (placeholder)
  dmd.clearScreen();
  dmd.drawString(2,  0, "SV");
  dmd.drawString(19, 0, "PV");
  itoa(Standard, svBuf, 10);
  dmd.drawString(2,  9, svBuf);
  dmd.drawString(19, 9, "--");   // PV belum tersedia

  Serial.println("DMD OK");
  delay(500); // Beri DMD waktu stabil sebelum I2C aktif

  // ── 2. Aktifkan I2C dengan clock rendah ──
  Wire.begin();
  Wire.setClock(50000); // 50kHz — lebih tahan noise dari P10

  // ── 3. Init MLX90614 dengan retry ──
  Serial.println("Init MLX90614...");
  for (int i = 0; i < 5; i++) {
    if (mlx.begin()) {
      sensorOK = true;
      break;
    }
    Serial.print("Retry MLX: ");
    Serial.println(i + 1);
    delay(300);
  }

  if (!sensorOK) {
    // Tampilkan error di P10 tapi TIDAK hang total
    // DMD tetap jalan, hanya sensor yang error
    dmd.clearScreen();
    dmd.drawString(2, 0, "Err");
    dmd.drawString(2, 8, "MLX");
    Serial.println("ERROR: MLX90614 gagal! Cek kabel & pull-up resistor.");
    // Loop error tapi relay tetap aman (OFF)
    while (1) {
      // Kedipkan tanda error di serial
      delay(2000);
      Serial.println("MLX90614 tidak terdeteksi. System halt.");
    }
  }

  Serial.println("MLX90614 OK");

  // ── 4. Tampilan header final (sensor sudah siap) ──
  dmd.clearScreen();
  dmd.drawString(2,  0, "SV");
  dmd.drawString(19, 0, "PV");
  itoa(Standard, svBuf, 10);
  dmd.drawString(2,  9, svBuf);

  Serial.println("== System Ready ==");
}

// ======================================================
void loop() {
  unsigned long currentMillis = millis();

  // ===== Baca Sensor & Update Display =====
  if (currentMillis - lastReadTime >= INTERVAL_READ) {
    lastReadTime = currentMillis;

    // Baca suhu
    float tempReading = mlx.readObjectTempC();
    int temp = (int)tempReading;

    Serial.print("Raw Temp: ");./
    Serial.print(tempReading, 1);
    Serial.println(" C");

    // Validasi rentang
    if (temp > TEMP_MIN && temp < TEMP_MAX) {
      memory = temp; // Simpan bacaan valid terakhir
    } else {
      temp = memory; // Gunakan memori jika bacaan ngaco
      Serial.println("Peringatan: Bacaan invalid, pakai nilai memori.");
    }

    // Update display HANYA jika nilai berubah (cegah flicker)
    if (temp != lastDisplayedTemp) {
      lastDisplayedTemp = temp;

      // Hapus area PV saja, header SV tidak disentuh
      dmd.drawFilledBox(19, 9, 31, 15, GRAPHICS_OFF);

      itoa(temp, tempBuf, 10);
      dmd.drawString(19, 9, tempBuf);

      Serial.print("Display PV update: ");
      Serial.println(temp);
    }

    // ===== Logika Kontrol Relay =====
    if (temp > Standard) {
      digitalWrite(Trigger_Lamp,  LOW);  // ON
      digitalWrite(Trigger_Alarm, LOW);  // ON
      Serial.println("Status: ALARM ON");
    } else {
      digitalWrite(Trigger_Lamp,  HIGH); // OFF
      digitalWrite(Trigger_Alarm, HIGH); // OFF
    }
  }

  // ===== Auto Reset setiap 10 menit =====
  if (currentMillis - lastResetTime >= INTERVAL_RESET) {
    Serial.println("Auto Reset...");
    resetFunc();
  }
}
