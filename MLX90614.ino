//======= sensor MLX90614 DCI I2C =======
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//======= display dot matrix =======
#include <DMD2.h>
#include <SPI.h>
#include <fonts/SystemFont5x7.h>
SoftDMD dmd(1, 1);
DMD_TextBox box(dmd, 1, 1, 32, 16);

//======= lamp =======
const int Trigger_Lamp =  5;
const int Trigger_Alarm =  4;

//======= batas =======
int Standard;
long interval = 5;  // sec
long interval_reset = 600;  // sec
long timer;
long timer_reset;
int memory;
unsigned long oldTime;

void(* resetFunc) (void) = 0;

void setup(){
  Serial.begin(9600);         
  pinMode(Trigger_Lamp, OUTPUT);
  pinMode(Trigger_Alarm, OUTPUT);
  
  dmd.setBrightness(150);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();

  if (!mlx.begin()) {
    dmd.begin();
    dmd.drawString(2, 0, String("St"));
    dmd.drawString(19, 0, String("Pv"));
    dmd.drawString(2, 8, String("25"));
    dmd.drawString(19, 8, String("&&"));
    //dmd.drawString(19, 8, String("14"));
    // dmd.drawString(1, 1, String("error"));
    Serial.println("Error caonnecting to MLX sensor. Check wiring.");
    while (1);
  };
}

void loop() {
  if ((millis() - oldTime) > 1000) {
    int temp = mlx.readObjectTempC();
    dmd.begin();
    oldTime = millis();

    if (temp > 2 && temp < 50) {
      memory = temp;
      dmd.drawString(2, 0, String("SV"));
      dmd.drawString(19, 0, String("PV"));
      dmd.drawString(2, 9, String("25"));
      dmd.drawString(19, 9, String(temp));
    } else {
      dmd.drawString(19, 9, String(memory));
    }

    Serial.print(mlx.readObjectTempC());
    
    Standard = 20;
    if (temp > Standard) {
      digitalWrite(Trigger_Lamp, LOW );
      digitalWrite(Trigger_Alarm, LOW );
    } else {
      digitalWrite(Trigger_Lamp, HIGH);
      digitalWrite(Trigger_Alarm, HIGH);
    }
  }
  if ((timer_reset + interval_reset * 1000) < millis()) {
    timer_reset = millis();
    Serial.println("reset");
    delay (100);
    resetFunc();
  }
}
