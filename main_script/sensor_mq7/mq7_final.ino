#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>


int sensorGasPin = A0;
int buzzerPin = 2;

int sensorValue = 0;
float voltage = 0;
float ppm = 0;
int warningCount = 0;

// Konstanta kalibrasi
const float RL = 10.0;
const float R0 = 1.56;

// Variabel untuk non-blocking buzzer
unsigned long previousMillis = 0;
unsigned long buzzerInterval = 0;
bool buzzerState = false;
int currentMode = 0; // 0=aman, 1=waspada, 2=bahaya

// Alamat I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int chipSelect = 10; // Pin CS SD Card
File dataFile;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);


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
  inisialisasiSDCard();
  delay(2000);
}

void loop() {
  // Baca sensor dan hitung PPM
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead >= 1000) { // Baca sensor setiap 1 detik
    lastSensorRead = millis();
    
    int nilaiSensor = analogRead(sensorGasPin);
    voltage = nilaiSensor * (5.0 / 1023.0);
    float RS = ((5.0 * RL) / voltage) - RL;
    float ratio = RS / R0;
    
    ppm = pow((ratio / 0.5), -1.5) * 100;
    if(ppm < 0) ppm = 0;

    // Update LCD
    Serial.print("PPM : ");
    Serial.println(ppm);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PPM : ");
    lcd.print(ppm);

    // Tentukan mode berdasarkan PPM
    lcd.setCursor(0, 1);
    dataFile = SD.open("log.csv", FILE_WRITE);
    if(ppm >= 20 && ppm <= 1000){
      lcd.print("WASPADA!!");
      currentMode = 1;
      buzzerInterval = 500; // 500ms interval untuk waspada
      if (dataFile) {
        dataFile.print(ppm);
        dataFile.print(",");
        dataFile.println("Waspada");
        dataFile.close();
        Serial.println("Data disimpan ke SD card.");
      } else {
        Serial.println("Gagal menulis ke file log.csv.");
      }
    }
    else if(ppm > 1000){
      lcd.print("BAHAYA!!");
      currentMode = 1;
      buzzerInterval = 200; // 200ms interval untuk bahaya
      if (dataFile) {
        dataFile.print(ppm);
        dataFile.print(",");
        dataFile.println("Bahaya");
        dataFile.close();
        Serial.println("Data disimpan ke SD card.");
      } else {
        Serial.println("Gagal menulis ke file log.csv.");
      }
    }
    else {
      lcd.print("AMAN");
      currentMode = 0;
      digitalWrite(buzzerPin, LOW); // Matikan buzzer
      if (dataFile) {
        dataFile.print(ppm);
        dataFile.print(",");
        dataFile.println("Aman");
        dataFile.close();
        Serial.println("Data disimpan ke SD card.");
      } else {
        Serial.println("Gagal menulis ke file log.csv.");
      }
    }
  }

  // Kontrol buzzer non-blocking
  handleBuzzer();
}

void handleBuzzer() {
  unsigned long currentMillis = millis();
  if (currentMode == 0) {
    // Mode aman - buzzer mati
    digitalWrite(buzzerPin, LOW);
    buzzerState = false;
  }
  else if (currentMode == 1) {
    // Mode waspada atau bahaya - buzzer berkedip
    if (currentMillis - previousMillis >= buzzerInterval) {
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(buzzerPin, buzzerState ? HIGH : LOW);
    }
  }
}

// Fungsi kalibrasi sensor (tidak berubah)
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
    
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print((i+1)*2);
    lcd.print("%");
    delay(100);
  }
  
  float avgRS = total / 50;
  float newR0 = avgRS / 0.5;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Complete!");
  Serial.print("Nilai R0 kalibrasi: ");
  Serial.println(newR0);
  delay(2000);
}

void inisialisasiSDCard(){
  if (!SD.begin(chipSelect)) {
    Serial.println("Gagal menginisialisasi SD card.");
    while (1); // Hentikan program
  }

  Serial.println("SD card berhasil diinisialisasi.");

  // Buat header jika file belum ada
  if (!SD.exists("log.csv")) {
    dataFile = SD.open("log.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Nilai PPM,Status");
      dataFile.close();
      Serial.println("File log.csv dibuat dengan header.");
    }
  }
}
