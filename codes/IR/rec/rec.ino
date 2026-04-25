const int irPin = A0;

const int numReadings = 500;    // Size of the buffer
int readings[numReadings];      // Circular buffer for ADC readings
int readIndex = 0;              
long total = 0;                 
int averageNoise = 0;           // V_noise
int signalMargin = 30;          // Sigma (Assumed margin for 1 M-Ohm setup)
int dynamicThreshold = 0;       // V_threshold

unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 50; 
bool bufferFilled = false;

// ==========================================
// Morse Code Decoding Variables
// ==========================================
const int Tu = 200; // Base time unit in milliseconds

// Tolerance windows for physical/hardware latency
const int ditMin = Tu * 0.3;
const int ditMax = Tu * 1.5;
const int dahMin = Tu * 1.6;
const int dahMax = Tu * 4.0;
const int charGapMin = Tu * 2.0;
const int wordGapMin = Tu * 5.0;

bool isReceiving = false;
unsigned long stateStartTime = 0;
String currentSymbol = ""; // Stores dots/dashes (e.g., ".-")

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);

  // Initialize all readings to 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
  
  Serial.println("System Booting...");
}

void loop() {
  unsigned long currentTime = millis();

  // 1. NON-BLOCKING FIXED-INTERVAL SAMPLING
  if (currentTime - lastSampleTime >= sampleInterval) {
    lastSampleTime = currentTime;

    int sensorValue = analogRead(irPin); // V_adc

    // Update Circular Buffer
    total = total - readings[readIndex];
    readings[readIndex] = sensorValue;
    total = total + readings[readIndex];
    
    readIndex = (readIndex + 1) % numReadings;

    // FIX: Set bufferFilled to true once we loop back to 0 for the first time
    if (readIndex == 0 && !bufferFilled) {
      bufferFilled = true;
      Serial.println("Buffer filled. Ready to receive!");
    }

    // 2. SDM ALGORITHM CALCULATIONS
    averageNoise = total / numReadings;                 // Eq. 1: V_noise
    dynamicThreshold = averageNoise + signalMargin;     // Eq. 3: V_threshold

    // Do not process logic until baseline stabilizes
    if (!bufferFilled) return;

    // 3. ADAPTIVE HIGH-PASS COMPARATOR
    bool currentLogicState = (sensorValue > dynamicThreshold);

    // 4. MORSE CODE STATE MACHINE
    // State Change: LOW to HIGH (Pulse Started)
    if (currentLogicState && !isReceiving) {
      unsigned long gapDuration = currentTime - stateStartTime;
      isReceiving = true;
      stateStartTime = currentTime;

      // Evaluate the gap that just ended
      if (gapDuration > wordGapMin && currentSymbol.length() == 0) {
        Serial.print(" "); // Inter-word gap
      } 
      else if (gapDuration > charGapMin && currentSymbol.length() > 0) {
        decodeCharacter(currentSymbol);
        currentSymbol = ""; // Reset for next character
      }
    }
    
    // State Change: HIGH to LOW (Pulse Ended)
    else if (!currentLogicState && isReceiving) {
      unsigned long pulseDuration = currentTime - stateStartTime;
      isReceiving = false;
      stateStartTime = currentTime;

      // Evaluate the pulse that just ended
      if (pulseDuration >= ditMin && pulseDuration <= ditMax) {
        currentSymbol += ".";
      } 
      else if (pulseDuration >= dahMin && pulseDuration <= dahMax) {
        currentSymbol += "-";
      }
    }
  }

  // 5. TIMEOUT CATCHER (Flushes last character if transmission stops completely)
  if (!isReceiving && currentSymbol.length() > 0) {
    if (millis() - stateStartTime > wordGapMin) {
      decodeCharacter(currentSymbol);
      currentSymbol = "";
    }
  }
}

// ==========================================
// Dictionary Function
// ==========================================
void decodeCharacter(String morse) {
  if (morse == ".-") Serial.print("A");
  else if (morse == "-...") Serial.print("B");
  else if (morse == "-.-.") Serial.print("C");
  else if (morse == "-..") Serial.print("D");
  else if (morse == ".") Serial.print("E");
  else if (morse == "..-.") Serial.print("F");
  else if (morse == "--.") Serial.print("G");
  else if (morse == "....") Serial.print("H");
  else if (morse == "..") Serial.print("I");
  else if (morse == ".---") Serial.print("J");
  else if (morse == "-.-") Serial.print("K");
  else if (morse == ".-..") Serial.print("L");
  else if (morse == "--") Serial.print("M");
  else if (morse == "-.") Serial.print("N");
  else if (morse == "---") Serial.print("O");
  else if (morse == ".--.") Serial.print("P");
  else if (morse == "--.-") Serial.print("Q");
  else if (morse == ".-.") Serial.print("R");
  else if (morse == "...") Serial.print("S");
  else if (morse == "-") Serial.print("T");
  else if (morse == "..-") Serial.print("U");
  else if (morse == "...-") Serial.print("V");
  else if (morse == ".--") Serial.print("W");
  else if (morse == "-..-") Serial.print("X");
  else if (morse == "-.--") Serial.print("Y");
  else if (morse == "--..") Serial.print("Z");
  else Serial.print("?"); // Error handler for corrupted pulses
}