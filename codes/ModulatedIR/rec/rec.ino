// --- Software-Defined IR Receiver (2kHz Goertzel) ---
// With Anti-Fragmentation Debounce and Standard Parallel LCD Output

#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
// RS=12, EN=11, D4=5, D5=4, D6=3, D7=2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int irPin = A0;

// Goertzel Algorithm Variables
const float TARGET_FREQ = 2000.0; 
const int N_SAMPLES = 80;         
float coeff;                      

const bool DEBUG_MODE = false; 
const float MAGNITUDE_THRESHOLD = 15000.0; 

// Morse Decoding Variables
unsigned long startTime = 0;
unsigned long darkStartTime = 0;
unsigned long lastHighTime = 0; 
bool isSignalActive = false;
bool spaceSent = true; 
String currentLetter = "";

// LCD Cursor Management
int cursorCol = 0;
int cursorRow = 0;

String morseDict[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", 
  "...--", "....-", ".....", "-....", "--...", "---..", "----."
};

void setup() {
  Serial.begin(9600);
  pinMode(irPin, INPUT);
  
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Listening...");
  delay(1500);
  lcd.clear();
  
  // Speed up the analogRead() from 100us to ~16us
  ADCSRA = (ADCSRA & 0xf8) | 0x04;

  // Auto-Calibrate the Sampling Frequency
  unsigned long startT = micros();
  for(int i = 0; i < N_SAMPLES; i++) {
    analogRead(irPin);
  }
  unsigned long endT = micros();
  float samplingFreq = (1000000.0 * N_SAMPLES) / (endT - startT);
  
  // Calculate Goertzel Math
  int k = (int)(0.5 + ((N_SAMPLES * TARGET_FREQ) / samplingFreq));
  float omega = (2.0 * PI * k) / N_SAMPLES;
  coeff = 2.0 * cos(omega);

  Serial.println("System Ready.");
}

char decodeMorse(String morse) {
  int i = 0;
  for (i = 0; i < 26; i++) {
    if (morseDict[i] == morse) return (char)('A' + i);
  }
  for (i = 26; i < 35; i++) {
    if (morseDict[i] == morse) return (char)('0' + i - 26);
  }
  return '?'; 
}

// Function to print to LCD and manage wrapping
void printToLCD(char c) {
  lcd.setCursor(cursorCol, cursorRow);
  lcd.print(c);
  cursorCol++;

  // Wrap to next line or clear screen if full
  if (cursorCol >= 16) {
    cursorCol = 0;
    cursorRow++;
    if (cursorRow >= 2) {
      delay(1000); // Pause briefly before clearing full screen
      lcd.clear();
      cursorRow = 0;
    }
  }
}

void loop() {
  // --- 1. THE GOERTZEL FILTER ---
  float q1 = 0.0;
  float q2 = 0.0;
  float q0;
  
  for(int i = 0; i < N_SAMPLES; i++) {
    q0 = coeff * q1 - q2 + (float)analogRead(irPin);
    q2 = q1;
    q1 = q0;
  }
  
  float magnitudeSquared = (q1 * q1) + (q2 * q2) - (coeff * q1 * q2);

  if (DEBUG_MODE) {
    Serial.println(magnitudeSquared);
    delay(50); 
    return;    
  }

  // --- 2. DEBOUNCED MORSE DECODING LOGIC ---
  bool currentPulse = (magnitudeSquared > MAGNITUDE_THRESHOLD);

  if (currentPulse) {
    if (!isSignalActive) {
      isSignalActive = true;
      startTime = millis();
      spaceSent = false;
    }
    lastHighTime = millis(); 
    
  } else {
    if (isSignalActive) {
      if (millis() - lastHighTime > 30) { 
        isSignalActive = false; 
        
        unsigned long duration = lastHighTime - startTime;
        darkStartTime = lastHighTime; 
        
        if (duration >= 50 && duration <= 350) { 
          currentLetter += "."; 
        }
        else if (duration > 350) { 
          currentLetter += "-"; 
        }
      }
    } else {
      unsigned long darkDuration = millis() - darkStartTime;
      
      // Print the letter if the gap is long enough
      if (darkDuration > 450 && currentLetter != "") {
        char decoded = decodeMorse(currentLetter);
        Serial.print(decoded);
        printToLCD(decoded);
        currentLetter = ""; 
      }
      
      // Print a space if the gap is a word gap
      if (darkDuration > 1500 && !spaceSent) {
        Serial.print(" ");
        printToLCD(' ');
        spaceSent = true;
      }
    }
  }
}
