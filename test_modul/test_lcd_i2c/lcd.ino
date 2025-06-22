#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Alamat I2C biasa untuk LCD 16x2: 0x27 atau 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);  // alamat, kolom, baris

void setup() {
  // put your setup code here, to run once:
  lcd.init();            // Inisialisasi LCD
  lcd.backlight();       // Nyalakan backlight 
  lcd.clear();

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
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Test LCD");
  lcd.setCursor(0,1);
  lcd.print("BERHASIL");
}
