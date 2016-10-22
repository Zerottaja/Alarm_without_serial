#include <LiquidCrystal.h>

// Määritellään I/O liitännät
LiquidCrystal lcd(12,11,5,4,3,2);

const int Led1Pin = 6;
const int Led2Pin = 9;
const int Led3Pin = 10;

const int Button1Pin = 7;
const int Button2Pin = 8;
const int Button3Pin = 13;

const int Sensor1Pin = A0;
const int Sensor2Pin = A1;
const int Sensor3Pin = A2;

//Alustetaan globaaleja muuttujia
bool alarm_on = false;
bool alarm_triggered = false;
bool true_alarm = false;

int Button1State = 0;
int Button2State = 0;
int Button3State = 0;

int last1 = 0;
int last2 = 0;
int last3 = 0;

unsigned long muutosaika;
unsigned long triggeraika;

bool Lippu1 = false;
bool Lippu1_ini = false;
bool Lippu2 = false;
bool Lippu2_ini = false;
bool Lippu3 = false;
bool Lippu3_ini = false;

void setup() {
  
  pinMode(Led1Pin, OUTPUT);
  pinMode(Led2Pin, OUTPUT);
  pinMode(Led3Pin, OUTPUT);
  pinMode(Button1Pin, INPUT);
  pinMode(Button2Pin, INPUT);
  pinMode(Button3Pin, INPUT);  

  // Alussa ledit ovat pois päältä
  digitalWrite(Led1Pin, LOW);
  digitalWrite(Led2Pin, LOW);
  digitalWrite(Led3Pin, LOW);

  // Käynnistetään lcd-näyttö
  lcd.begin(16,2);
}

void loop() {

  // Tyhjennetään lcd-näyttö ja luetaan nappulat
  lcd.clear();
  debounce_buttons();
    
  // Käyttäjä saa nappuloiden painamisesta LED-vasteen, vain jos hälytin ei ole lauennut
  if (!true_alarm) {
    digitalWrite(Led1Pin, Button1State);
    digitalWrite(Led2Pin, Button2State);
    digitalWrite(Led3Pin, Button3State);
  }

  if (alarm_on){
    // Tässä lohkossa käsitellään tilannetta, jossa hälytin on kytketty päälle.

    // Sensorien laukeaminen laukaisee hälyttimen:
    if (sensorilukija()) {
      if (!alarm_triggered) {
        triggeraika = millis();
      }
      alarm_triggered = true;
    }

    if (alarm_triggered && (millis() - triggeraika) > 10000) {
      // Tässä lohkossa käsitellään tilannetta, jossa hälytin on lauennut.
      true_alarm = true;
      lcd.print("ALARM!");
      lcd.setCursor(0,1);
      lcd.print("ALARM!");
      kaikki_ledit(true);
    } else {
      lcd.print("Kytketty!");
    }

    // Jos purkusekvenssi onnistuu, kytketään hälytin pois päältä
    if (purkusekvenssi()) {
      alarm_on = false;
      alarm_triggered = false;
      true_alarm = false;
      kaikki_ledit(false);
    }
    
  } else {
    // Tässä lohkossa käsitellään tilannetta, jossa hälytintä ei ole kytketty päälle.
    lcd.print("Ei kytketty.");

    // Pitämällä nappuloita 1 ja 2 kaksi sekuntia pohjassa, voidaan hälytin kytkeä päälle.
    if (Button1State == HIGH && Button2State == HIGH && Button3State == LOW && (millis() - muutosaika) > 2000) {
      alarm_on = true;
      kaikki_ledit(true);
      lcd.print("Kytkeytyy 30s");
      lcd.setCursor(0,1);
      lcd.print("kuluttua!");
            
      delay(3000);
      kaikki_ledit(false);
    }
  }
}

void debounce_buttons() {
  // Luetaan nappulat, onko muutoksia sitten viime kierroksen?
  if (last1 != digitalRead(Button1Pin) ||
      last2 != digitalRead(Button2Pin) ||
      last3 != digitalRead(Button3Pin) ) {
        
    muutosaika = millis();
    
  }

  if (millis() - muutosaika > 10) {
    Button1State = digitalRead(Button1Pin);
    Button2State = digitalRead(Button2Pin);
    Button3State = digitalRead(Button3Pin);
    }

  last1 = digitalRead(Button1Pin);
  last2 = digitalRead(Button2Pin);
  last3 = digitalRead(Button3Pin);

  return;
}

void kaikki_ledit(bool tila) {
  if (tila) {
    digitalWrite(Led1Pin, HIGH);
    digitalWrite(Led2Pin, HIGH);
    digitalWrite(Led3Pin, HIGH); 
  } else {
    digitalWrite(Led1Pin, LOW);
    digitalWrite(Led2Pin, LOW);
    digitalWrite(Led3Pin, LOW);
  }
  return;
}

bool sensorilukija() {
  if (analogRead(A0) < 500) {
    return true;
  } else {
    return false;
  }
}

bool purkusekvenssi() {

  if (Lippu1 == false && Lippu2 == false && Lippu3 == false){
    // Näppäinten 1 ja 2 combo aloittaa vaiheen yksi
    if(Button1State == HIGH && Button2State == HIGH && Button3State == LOW) {

      Lippu1_ini = true;

    // Vasta näppäimistä irti päästäminen aktivoi lipun 1
    } else if (Lippu1_ini == true && Button1State == LOW && Button2State == LOW && Button3State == LOW) {
        Lippu1_ini = false;
        Lippu1 = true;
    // Väärän napin painaminen heittää takaisin sekvenssin alkuun.
    } else if (Button3State == true) {
        Lippu1_ini = false;
    }
    
  } else if (Lippu1 == true && Lippu2 == false && Lippu3 == false) {
    // Näppäinten 1 ja 3 combo aloittaa vaiheen kaksi
    if(Button1State == HIGH && Button2State == LOW && Button3State == HIGH) {

      Lippu2_ini = true;

    // Vasta näppäimistä irti päästäminen aktivoi lipun 2
    } else if (Lippu2_ini == true && Button1State == LOW && Button2State == LOW && Button3State == LOW) {
        Lippu2_ini = false;
        Lippu1 = false;
        Lippu2 = true;
    // Väärän napin painaminen heittää takaisin sekvenssin alkuun.
    } else if (Button2State == true) {
        Lippu2_ini = false;
        Lippu1 = false;
    }
    
  } else if (Lippu1 == false && Lippu2 == true && Lippu3 == false) {
    // Näppäinten 2 ja 3 combo aloittaa vaiheen kolme
    if(Button1State == LOW && Button2State == HIGH && Button3State == HIGH) {

      Lippu3_ini = true;

    // Vasta näppäimistä irti päästäminen aktivoi lipun 3
    } else if (Lippu3_ini == true && Button1State == LOW && Button2State == LOW && Button3State == LOW) {
        Lippu3_ini = false;
        Lippu2 = false;
        Lippu3 = true;
    // Väärän napin painaminen heittää takaisin sekvenssin alkuun.
    } else if (Button1State == true) {
        Lippu3_ini = false;
        Lippu2 = false;
    }
  }

  // Jos näppäinyhdistelmä onnistuu kokonaan, alustetaan liput ja palautetaan true
  if (Lippu3) {
    Lippu3 = false;
    return true;
  // Jos vielä tällä kierroksella ei onnistuttu, palautetaan false
  } else {
    return false;
  }
}

