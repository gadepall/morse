const int irPin = A0;
const int numReadings = 10;
int readings[numReadings];      
int readIndex = 0;              
long total = 0;                  
int averageNoise = 0;                
const int signalMargin = 30;
int dynamicThreshold = 0;

void loop() {
  int sensorValue = analogRead(irPin);
  total = total - readings[readIndex];
  readings[readIndex] = sensorValue;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageNoise = total / numReadings;
  dynamicThreshold = averageNoise + signalMargin;

  if (sensorValue > dynamicThreshold) {
    // Edge detection logic
  }
}
