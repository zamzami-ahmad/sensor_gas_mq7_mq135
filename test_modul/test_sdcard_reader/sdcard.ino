#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  Serial.println("=== SD CARD TEST ===");
  
  // Test inisialisasi dengan berbagai pin
  int pins[] = {4, 8, 10, 53}; // Pin yang umum digunakan
  int numPins = sizeof(pins) / sizeof(pins[0]);
  
  for (int i = 0; i < numPins; i++) {
    Serial.print("Testing pin ");
    Serial.print(pins[i]);
    Serial.print("... ");
    
    if (SD.begin(pins[i])) {
      Serial.println("BERHASIL!");
      Serial.print("SD Card Type: ");
      
      Sd2Card card;
      SdVolume volume;
      
      if (card.init(SPI_HALF_SPEED, pins[i])) {
        Serial.print("SD");
        Serial.println(card.type() == SD_CARD_TYPE_SD1 ? "1" : 
                      card.type() == SD_CARD_TYPE_SD2 ? "2" : "HC");
        
        if (volume.init(card)) {
          Serial.print("Volume size (MB): ");
          Serial.println(volume.blocksPerCluster() * volume.clusterCount() * 512 / 1000000.0);
        }
      }
      return;
    } else {
      Serial.println("gagal");
    }
    delay(100);
  }
  
  Serial.println("Semua pin gagal. Periksa wiring dan SD card.");
}

void loop() {
  // Kosong
}
