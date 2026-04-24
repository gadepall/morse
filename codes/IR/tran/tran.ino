const int irLedPin = 5; 
const int ditTime = 200;
const int dahTime = 600;
const int symbolGap = 200;
const int letterGap = 800;
const int wordGap = 2000;

void setup() { 
  pinMode(irLedPin, OUTPUT); 
  Serial.begin(9600);
}

void flash(int duration) {
  digitalWrite(irLedPin, HIGH);
  delay(duration);
  digitalWrite(irLedPin, LOW);
  delay(symbolGap);
}

void sendMorse(char c) {
  c = toupper(c);
  switch (c) {
    case 'A': flash(ditTime); flash(dahTime); break;
    case 'B': flash(dahTime); flash(ditTime); flash(ditTime); flash(ditTime); break;
    // ... Additional cases omitted for brevity ...
    case 'Z': flash(dahTime); flash(dahTime); flash(ditTime); flash(ditTime); break;
    case ' ': delay(wordGap - symbolGap); break;
  }
  delay(letterGap - symbolGap);
}

void loop() {
  String input = "HELLO WORLD";
  input.trim();
  for (int i = 0; i < input.length(); i++) { sendMorse(input[i]); }
  delay(15000UL);
}
