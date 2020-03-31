int upper_threshold = 518;
int lower_threshold = 490;
int reading = 0;
float bpm = 0.0;
bool ignore_reading = false;
bool first_pulse_detected = false;
unsigned long first_pulse_time = 0;
unsigned long second_pulse_time = 0;
unsigned long pulse_interval = 0;

void setup(){
    Serial.begin(9600);
}

void loop(){
    reading = analogRead(0);

    if (reading > upper_threshold && ignore_reading == false) {
        if (first_pulse_detected == false) {
	          first_pulse_time = millis();
	          first_pulse_detected = true;
        } else {
	          second_pulse_time = millis();
	          pulse_interval = second_pulse_time - first_pulse_time;
	          first_pulse_time = second_pulse_time;
        }

        ignore_reading = true;
    }

    if (reading < lower_threshold) {
        ignore_reading = false;
    }

    bpm = (1.0/pulse_interval) * 60.0 * 1000;
    // uncomment these lines in case you want to view the various values in the console.....
    /*Serial.print(reading);
    Serial.print("\t");
    Serial.print(pulse_interval);
    Serial.print("\t");
    */
    Serial.print(bpm);
    Serial.println(" bpm");
    Serial.flush();
}
