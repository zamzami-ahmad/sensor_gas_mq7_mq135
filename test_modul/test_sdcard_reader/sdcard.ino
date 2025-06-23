#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // Pin CS SD Card
File dataFile;

void setup() {
  Serial.begin(9600);

   if (!SD.begin(chipSelect)) {
    Serial.println("Gagal menginisialisasi SD card.");
    while (1); // Hentikan program
  }

  Serial.println("SD card berhasil diinisialisasi.");

  // Buat header jika file belum ada
  if (!SD.exists("log.csv")) {
    dataFile = SD.open("log.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("ID,Nilai,Status");
      dataFile.close();
      Serial.println("File log.csv dibuat dengan header.");
    }
  }
}


void loop() {
  //menyimpan data secara manual
  dataFile = SD.open("log.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(1);
    dataFile.print(",");
    dataFile.print(500);
    dataFile.print(",");
    dataFile.println("Bahaya");
    dataFile.close();
    Serial.println("Data disimpan ke SD card.");
  } else {
    Serial.println("Gagal menulis ke file log.csv.");
  }

  delay(5000);
}

