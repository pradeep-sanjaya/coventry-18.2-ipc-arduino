#define HEART_IN A0
#define HEART_THRESHOLD 480
#define VIBRATOR_OUT 2

float heart_value = 0.0;

void setup() {
    Serial.begin(9600);
    pinMode(HEART_IN, INPUT);
}

void loop() {
  
    // read heartbeat
    heart_value = analogRead(HEART_IN);
    Serial.println(heart_value);

    // vibrate if reading is greater than threshold
    if (heart_value > HEART_THRESHOLD) {
        vibrate();
    }
    
    delay(1000);
}

void vibrate() {
    Serial.println("... vibrating ...");
    
    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(100);
    
    digitalWrite(VIBRATOR_OUT, LOW);
    delay(100);

    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(200);
    
    digitalWrite(VIBRATOR_OUT, LOW);
    delay(100);

    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(200);

    digitalWrite(VIBRATOR_OUT, LOW);
}
