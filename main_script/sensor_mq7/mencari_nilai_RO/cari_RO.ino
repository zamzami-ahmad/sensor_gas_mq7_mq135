#define RL 10.0  // Load resistor dalam kOhm (moul MQ-7: 10kΩ)

float analogValue = 0;
float VRL = 0;
float Rs = 0;
float Ro = 0;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Baca nilai analog dari pin A0 (0–1023)
  analogValue = analogRead(A0);

  // Konversi nilai analog ke tegangan (0–5V)
  VRL = analogValue * (5.0 / 1023.0);

  // Hitung nilai Rs (sensor resistance)
  Rs = ((5.0 / VRL) - 1.0) * RL;

  // Kalibrasi Ro (di udara bersih)
  // Untuk MQ-7, Rs/Ro ~ 27 di udara bersih (dari datasheet)
  Ro = Rs / 27;

  // Tampilkan hasil
  Serial.print("Nilai analog: ");
  Serial.print(analogValue);
  Serial.print(" | VRL: ");
  Serial.print(VRL, 2);
  Serial.print(" V | Rs: ");
  Serial.print(Rs, 2);
  Serial.print(" kOhm | Ro: ");
  Serial.println(Ro, 2);

  delay(1000);
}
