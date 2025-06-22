
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int sensorGasPin = A0;
int buzzerPin = 2;

int sensorValue = 0;
float voltage = 0;
float ppm = 0;
int warningCount = 0;

// Konstanta kalibrasi (perlu disesuaikan berdasarkan datasheet)
const float RL = 10.0;        // Load resistance (10kΩ)
const float R0 = 10.0;        // Sensor resistance di udara bersih


// Alamat I2C biasa untuk LCD 16x2: 0x27 atau 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);  // alamat, kolom, baris

void setup() {
  lcd.init();            // Inisialisasi LCD
  lcd.backlight();       // Nyalakan backlight 
  lcd.clear();
  pinMode(buzzerPin, OUTPUT);

  // Cek koneksi I2C LCD
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(0x27);
  if(Wire.endTransmission() == 0) {
    Serial.println("LCD I2C found at 0x27");
  } else {
    Serial.println("LCD I2C not found at 0x27, trying 0x3F...");
    Wire.beginTransmission(0x3F);
    if(Wire.endTransmission() == 0) {
      Serial.println("LCD I2C found at 0x3F");
    } else {
      Serial.println("LCD I2C not found!");
    }
  }

  
  
  calibrateSensor();
  delay(2000);
}

void loop() {

  int nilaiSensor = analogRead(sensorGasPin);

  // Konversi ke voltage (0-5V)
  voltage = nilaiSensor * (5.0 / 1023.0);

  // Hitung resistance sensor
  float RS = ((5.0 * RL) / voltage) - RL;
  
  // Hitung ratio RS/R0
  float ratio = RS / R0;
  
  // Konversi ke ppm CO (rumus aproksimasi)
  ppm = pow((ratio / 0.5), -1.5) * 100;
  if(ppm < 0) ppm = 0;  // Hindari nilai negatif

  // menampilkan di LCD
  Serial.print("PPM : ");
  Serial.println(ppm);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PPM : ");
  lcd.print(ppm);

  //kondisi nilai tekanan gas
  lcd.setCursor(0, 1);
  if(ppm > 50 && ppm <700){
    lcd.print("WASPADA!!");
    tone(buzzerPin, 3000, 1000);
  }
  else if(ppm > 700){
    lcd.print("BAHAYA!!");
    tone(buzzerPin, 3000, 250);
  }
  else {
    lcd.print("AMAN");
    noTone(buzzerPin);
  }
}

// Fungsi kalibrasi sensor (jalankan di udara bersih)
void calibrateSensor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kalibrasi...");
  
  Serial.println("Kalibrasi sensor di udara bersih...");
  float total = 0;
  
  for(int i = 0; i < 50; i++) {
    int val = analogRead(sensorGasPin);
    float volt = val * (5.0 / 1023.0);
    float rs = ((5.0 * RL) / volt) - RL;
    total += rs;
    
    // Progress di LCD
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print((i+1)*2);
    lcd.print("%");
    
    delay(100);
  }
  
  float avgRS = total / 50;
  float newR0 = avgRS / 0.5;  // Ratio di udara bersih
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Complete!");
  
  Serial.print("Nilai R0 kalibrasi: ");
  Serial.println(newR0);
  
  delay(2000);
}
