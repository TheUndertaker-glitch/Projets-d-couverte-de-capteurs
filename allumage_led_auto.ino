int lightPin = 0;        // Capteur de lumière sur A0
int latchPin = 11;       // Pin ST_CP du 74HC595
int clockPin = 9;        // Pin SH_CP du 74HC595
int dataPin = 12;        // Pin DS du 74HC595
int buzzerPin = 8;       // Pin du buzzer (broche libre)

int leds = 0;            // État actuel des LEDs
int currentLEDSLit = 0;  // Nombre de LEDs actuellement allumées
int previousLEDSLit = 0;  // Pour détecter les passages aux extrêmes

unsigned long previousMillis = 0;
const long interval = 100;  // Vitesse de transition (ms)

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Animation de démarrage (sans bip aux extrêmes pour ne pas sonner au boot)
  for (int i = 0; i <= 8; i++) {
    leds = (1 << i) - 1;
    updateShiftRegister();
    delay(100);
  }
  for (int i = 8; i >= 0; i--) {
    leds = (1 << i) - 1;
    updateShiftRegister();
    delay(100);
  }
  leds = 0;
  updateShiftRegister();
  
  previousLEDSLit = 0;
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}

void longBeep() 
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);  // Durée du bip : 1 s 
  digitalWrite(buzzerPin, LOW);
}

void loop()
{
  int reading = analogRead(lightPin);
  int targetLEDSLit = reading / 57;
  if (targetLEDSLit > 8) targetLEDSLit = 8;
  if (targetLEDSLit < 0) targetLEDSLit = 0;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Sauvegarde avant mise à jour
    previousLEDSLit = currentLEDSLit;

    // Transition douce
    if (currentLEDSLit < targetLEDSLit) {
      currentLEDSLit++;
    } else if (currentLEDSLit > targetLEDSLit) {
      currentLEDSLit--;
    }

    // Détection des extrêmes atteints
    if (currentLEDSLit == 0 && previousLEDSLit != 0) {
      longBeep();  // Bip quand on arrive au minimum (sombre)
    }
    else if (currentLEDSLit == 8 && previousLEDSLit != 8) {
      longBeep();  // Bip quand on arrive au maximum (clair)
    }

    // Mise à jour des LEDs
    leds = 0;
    for (int i = 0; i < currentLEDSLit; i++) {
      leds |= (1 << i);
    }
    updateShiftRegister();
  }
}