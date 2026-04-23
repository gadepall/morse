const int ledPin = 5;
const int ditTime = 200;
const int dahTime = 600;
const int symbolGap = 200;
const int letterGap = 600;
const int wordGap = 1400;

String letters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
};

void setup() { pinMode(ledPin, OUTPUT); }

void flash(int duration) {
  digitalWrite(ledPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  delay(symbolGap);
}

void sendChar(char c) {
  if (c >= 'A' && c <= 'Z') {
    String morse = letters[c - 'A'];
    for (int i = 0; i < morse.length(); i++) {
      if (morse[i] == '.') flash(ditTime);
      else if (morse[i] == '-') flash(dahTime);
    }
    delay(letterGap - symbolGap); 
  } 
  else if (c == ' ') { delay(wordGap - symbolGap); }
}

void loop() {
  String input = "HELLO FROM IIT HYDERABAD";
  input.toUpperCase();
  for (int i = 0; i < input.length(); i++) { sendChar(input[i]); }
  delay(5000);
}

