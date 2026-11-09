#include <Arduino.h>

const int LED_PINS[] = {13, 14, 27, 26, 25, 33, 32, 23};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

const int idle = 900;
const int RPM_MAX = 7000;
const int RPM_START = 3000;
const int RPM_SHIFT = 6200;
const int SAMPLE_INTERVAL_MS = 10;

const int SWEEP_RPM_PER_SEC = 2000;



int fake_rpm(){ //Sweeps up and down
  static int rpm = idle;
  static int direction = 1;
  int step = SWEEP_RPM_PER_SEC * SAMPLE_INTERVAL_MS / 1000;   // RPM per sample
 
  rpm += direction * step;
  if (rpm >= RPM_MAX) { rpm = RPM_MAX; direction = -1; }
  if (rpm <= idle)    { rpm = idle;    direction =  1; }
  return rpm;

}

int rpmToLEDCount(int rpm){
  int clampedRPM = constrain(rpm, RPM_START, RPM_SHIFT);
  return map(clampedRPM, RPM_START, RPM_SHIFT, 0, NUM_LEDS);
}

void showRPM(int rpm){
  if (rpm >= RPM_SHIFT){
    bool on = (millis()/60)%2; //8Hz flash
    for (int i = 0; i < NUM_LEDS; i++){
      digitalWrite(LED_PINS[i], on);
    }
    return;
  }
  
  int lit = rpmToLEDCount(rpm);
  for(int i = 0; i<NUM_LEDS; i++){
    digitalWrite(LED_PINS[i], i<lit ? HIGH : LOW);
  }
}

int readRPM(){
  return fake_rpm();
}

void setup(){
  Serial.begin(115200);
  for (int i =0; i<NUM_LEDS; i++){
    pinMode(LED_PINS[i], OUTPUT);
  }

  //INIT
  for(int i = 0; i< NUM_LEDS; i++){
    digitalWrite(LED_PINS[i], HIGH);
    delay(500);
  }
  delay (300);
  for (int i =0; i< NUM_LEDS; i++){
    digitalWrite(LED_PINS[i], LOW);
  }
}

void loop(){
  static unsigned long lastUpdate = 0, lastPrint = 0;
  static int rpm = idle;
  unsigned long now = millis();

  if (now - lastUpdate >= SAMPLE_INTERVAL_MS) {     
    lastUpdate = now;
    rpm = fake_rpm();
  }

  showRPM(rpm);

  if(now-lastPrint >=200){
    lastPrint = now;
    Serial.printf("RPM: %d\n", rpm);
  }
}