// Вариант 3: Серверная
// Датчики: NTC (температура 0-60 C) -> A0, MQ2 (дым 0-100 %) -> A1, PIR (присутствие) -> D2
// Исполнительное устройство: реле (вентилятор) -> D7, светодиод зелёный (150 Ом) -> D8
// Период опроса T = 3 с. Порог события: температура > 27 C

const unsigned long T_MS = 3000;     // период опроса, мс
const float BETA = 3950.0;           // коэффициент Beta NTC в Wokwi
const float SMOKE_MIN = 0.0;         // диапазон датчика 2, %
const float SMOKE_MAX = 100.0;
const float TEMP_THRESHOLD = 27.0;   // порог события, C

const uint8_t PIN_NTC   = A0;
const uint8_t PIN_MQ2   = A1;
const uint8_t PIN_PIR   = 2;
const uint8_t PIN_RELAY = 7;
const uint8_t PIN_LED   = 8;

unsigned long lastMs = 0;
unsigned long n = 0;
bool thresholdActive = false;

// Температура NTC из analogRead (формула Beta из документации Wokwi).
// У NTC в Wokwi значение analogRead падает при росте температуры.
float readTemperature() {
  int a = analogRead(PIN_NTC);
  if (a < 1) a = 1;
  if (a > 1022) a = 1022;
  return 1.0 / (log(1.0 / (1023.0 / a - 1.0)) / BETA + 1.0 / 298.15) - 273.15;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  unsigned long now = millis();
  if (now - lastMs < T_MS) return;
  lastMs += T_MS;
  n++;

  float s1 = readTemperature();
  // дым: значение = мин + (макс - мин) * analogRead / 1023
  float s2 = SMOKE_MIN + (SMOKE_MAX - SMOKE_MIN) * analogRead(PIN_MQ2) / 1023.0;
  int   s3 = digitalRead(PIN_PIR);

  Serial.print("DATA n=");  Serial.print(n);
  Serial.print(" s1=");     Serial.print(s1, 2);
  Serial.print(" s2=");     Serial.print(s2, 2);
  Serial.print(" s3=");     Serial.println(s3);

  bool over = (s1 > TEMP_THRESHOLD);
  if (over && !thresholdActive) {
    thresholdActive = true;
    digitalWrite(PIN_RELAY, HIGH);
    digitalWrite(PIN_LED, HIGH);
    Serial.print("EVENT n="); Serial.print(n); Serial.println(" type=THRESHOLD");
  } else if (!over && thresholdActive) {
    thresholdActive = false;
    digitalWrite(PIN_RELAY, LOW);
    digitalWrite(PIN_LED, LOW);
    Serial.print("EVENT n="); Serial.print(n); Serial.println(" type=NORMAL");
  }
}
