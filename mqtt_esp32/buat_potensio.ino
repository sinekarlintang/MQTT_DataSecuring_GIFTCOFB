// Menggunakan pin ADC 34 dan 35 pada ESP32 untuk membaca nilai potensiometer
const int steerPin = 34;
const int throttlePin = 35;
const float MAX_VOLTAGE = 3.3;

void setup() {
  Serial.begin(9600);

  // Konfigurasi ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("Ready to read potentiometers");
}

void loop() {
  // Membaca nilai potensiometer untuk steering
  int steerValue = analogRead(steerPin);
  float steerVoltage = steerValue * (MAX_VOLTAGE / 4095.0);

  // Menghitung sudut steering angle
  float maxSteerValue = (MAX_VOLTAGE / 2.0);
  float steerAngle = 0;
  
  if (steerVoltage <= maxSteerValue) {
    steerAngle = ((steerVoltage - maxSteerValue) * (90.0 / maxSteerValue));
  } else {
    steerAngle = ((steerVoltage - maxSteerValue) * (90.0 / maxSteerValue));
  }

  // Membaca nilai potensiometer untuk throttle
  int throttleValue = analogRead(throttlePin);
  float throttleVoltage = throttleValue * (MAX_VOLTAGE / 4095.0);
  
  // Memastikan bahwa nilai throttle tidak melebihi 100%
  float throttlePosition = (throttleVoltage * 100.0) / MAX_VOLTAGE;
  if (throttlePosition > 100.0) {
    throttlePosition = 100.0;
  }

  // Menampilkan nilai sudut steering dan posisi throttle pada Serial Monitor
  Serial.print("Steering angle: ");
  Serial.print(steerAngle);
  Serial.println(" degrees");
  Serial.print("Throttle position: ");
  Serial.print(throttlePosition);
  Serial.println(" %");

  // Menunggu sejenak sebelum membaca nilai potensiometer kembali
  delay(2000);
}
