const int ldrPin = A0;
const int threshold = 90;
unsigned long startTime = 0;
unsigned long darkStartTime = 0;
bool isLight = false;
String currentLetter = "";

void loop() {
  int sensorValue = analogRead(ldrPin);
  if (sensorValue > threshold) {
    if (!isLight) { startTime = millis(); isLight = true; }
    darkStartTime = millis(); 
  } else {
    if (isLight) {
      unsigned long duration = millis() - startTime;
      if (duration >= 100 && duration <= 350) currentLetter += ".";
      else if (duration >= 400 && duration <= 800) currentLetter += "-";
      isLight = false;
      darkStartTime = millis();
    } else {
      unsigned long darkDuration = millis() - darkStartTime;
      if (darkDuration > 500 && currentLetter != "") {
        Serial.print(decodeMorse(currentLetter));
        currentLetter = "";
      }
    }
  }
}
