const int irPin = A0;
const int numReadings = 20;    // Size of the rolling average window
int readings[numReadings];     
int readIndex = 0;             
long total = 0;                
int averageNoise = 0;          
const int signalMargin = 30;   // Sensitivity offset above the noise floor
int dynamicThreshold = 0;

// --- Morse Timing Variables ---
unsigned long startTime = 0;
unsigned long darkStartTime = 0;
bool isSignalActive = false;
String currentLetter = "";

// Morse Dictionary for Decoding
String morseDict[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};

void setup() {
  Serial.begin(9600);
  pinMode(irPin, INPUT);
  
  // Initialize SDM buffer with 0s
  for (int i = 0; i < numReadings; i++) readings[i] = 0;
  
  Serial.println("Phase II SDM Receiver Ready...");
}

char decodeMorse(String morse) {
  for (int i = 0; i < 26; i++) {
    if (morseDict[i] == morse) return (char)('A' + i);
  }
  return '?';
}

void loop() {
  int sensorValue = analogRead(irPin);

  total = total - readings[readIndex];
  readings[readIndex] = sensorValue;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  
  averageNoise = total / numReadings;
  
  dynamicThreshold = averageNoise + signalMargin;

  if (sensorValue > dynamicThreshold) {
    if (!isSignalActive) {
      isSignalActive = true;
      startTime = millis();
    }
  } else {
    if (isSignalActive) {
      isSignalActive = false;
      unsigned long duration = millis() - startTime;
      darkStartTime = millis();

      if (duration >= 50 && duration <= 350) {
        currentLetter += ".";
      } else if (duration > 350) {
        currentLetter += "-";
      }
    } else {
      unsigned long darkDuration = millis() - darkStartTime;
      
      if (darkDuration > 600 && currentLetter != "") {
        Serial.print(decodeMorse(currentLetter));
        currentLetter = "";
      }
      if (darkDuration > 1500 && darkDuration < 1550) {
        Serial.print(" ");
      }
    }
  }
}