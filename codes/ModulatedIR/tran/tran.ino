// --- Software-Defined IR Transmitter (2kHz Carrier) ---

const int irLedPin = 5; 
const int carrierFreq = 2000; // The target frequency for our Goertzel filter

// Standard Morse Code Timing (ms)
const int ditTime = 200;
const int dahTime = 600;
const int symbolGap = 200;
const int letterGap = 600;
const int wordGap = 1400;

void setup() { 
  pinMode(irLedPin, OUTPUT); 
  Serial.begin(9600);
  Serial.println("2kHz Modulated Transmitter Ready.");
}

// THIS IS THE MAGIC FUNCTION
void flash(int duration) {
  // Blast the IR LED on and off at exactly 2,000 Hz
  tone(irLedPin, carrierFreq); 
  
  // Keep blasting for the duration of the Dit or Dah
  delay(duration); 
  
  // Shut off the 2kHz strobe
  noTone(irLedPin); 
  
  // Standard gap between symbols
  delay(symbolGap); 
}

void sendMorse(char c) {
  c = toupper(c); // Make it case-insensitive
  
  switch (c) {
    case 'A': flash(ditTime); flash(dahTime); break;
    case 'B': flash(dahTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    case 'C': flash(dahTime); flash(ditTime); flash(dahTime); flash(ditTime); break;
    case 'D': flash(dahTime); flash(ditTime); flash(ditTime); break;
    case 'E': flash(ditTime); break;
    case 'F': flash(ditTime); flash(ditTime); flash(dahTime); flash(ditTime); break;
    case 'G': flash(dahTime); flash(dahTime); flash(ditTime); break;
    case 'H': flash(ditTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    case 'I': flash(ditTime); flash(ditTime); break;
    case 'J': flash(ditTime); flash(dahTime); flash(dahTime); flash(dahTime); break;
    case 'K': flash(dahTime); flash(ditTime); flash(dahTime); break;
    case 'L': flash(ditTime); flash(dahTime); flash(ditTime); flash(ditTime); break;
    case 'M': flash(dahTime); flash(dahTime); break;
    case 'N': flash(dahTime); flash(ditTime); break;
    case 'O': flash(dahTime); flash(dahTime); flash(dahTime); break;
    case 'P': flash(ditTime); flash(dahTime); flash(dahTime); flash(ditTime); break;
    case 'Q': flash(dahTime); flash(dahTime); flash(ditTime); flash(dahTime); break;
    case 'R': flash(ditTime); flash(dahTime); flash(ditTime); break;
    case 'S': flash(ditTime); flash(ditTime); flash(ditTime); break;
    case 'T': flash(dahTime); break;
    case 'U': flash(ditTime); flash(ditTime); flash(dahTime); break;
    case 'V': flash(ditTime); flash(ditTime); flash(ditTime); flash(dahTime); break;
    case 'W': flash(ditTime); flash(dahTime); flash(dahTime); break;
    case 'X': flash(dahTime); flash(ditTime); flash(ditTime); flash(dahTime); break;
    case 'Y': flash(dahTime); flash(ditTime); flash(dahTime); flash(dahTime); break;
    case 'Z': flash(dahTime); flash(dahTime); flash(ditTime); flash(ditTime); break;
    case ' ': delay(wordGap - symbolGap); break;
    case '1': flash(ditTime); flash(dahTime); flash(dahTime); flash(dahTime); flash(dahTime); break;
    case '2': flash(ditTime); flash(ditTime); flash(dahTime); flash(dahTime); flash(dahTime); break;
    case '3': flash(ditTime); flash(ditTime); flash(ditTime); flash(dahTime); flash(dahTime); break;
    case '4': flash(ditTime); flash(ditTime); flash(ditTime); flash(ditTime); flash(dahTime); break;
    case '5': flash(ditTime); flash(ditTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    case '6': flash(dahTime); flash(ditTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    case '7': flash(dahTime); flash(dahTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    case '8': flash(dahTime); flash(dahTime); flash(dahTime); flash(ditTime); flash(ditTime); break;
    case '9': flash(dahTime); flash(dahTime); flash(dahTime); flash(dahTime); flash(ditTime); break;
    case '0': flash(dahTime); flash(dahTime); flash(dahTime); flash(dahTime); flash(dahTime); break;
  }
  delay(letterGap - symbolGap);
}

void loop() {
  String input = "EE1003 SCIENTIFIC PROGRAMMING";
  input.trim();
  
  for (int i = 0; i < input.length(); i++) { 
    sendMorse(input[i]); 
  }
  
  delay(20000); // Wait 5 seconds before repeating
}
