#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define LED_ROUGE 13
#define LED_VERT 12
#define BUZZER 11

// Clavier 4x4
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// États
enum Etat { VERROUILLE, SAISIE_PIN, OUVERT, REFUS, ADMIN };
Etat etatActuel = VERROUILLE;

// Variables
String PIN_SECRET = "1234";
String saisieEnCours = "";
unsigned long tempsChangementEtat = 0;
const unsigned long DUREE_OUVERT = 3000;
const unsigned long DUREE_REFUS  = 1000;

// Fonctions affichage
void afficherVerrouille() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("** VERROUILLE **");
  lcd.setCursor(0, 1);
  lcd.print("  * pour PIN    ");
  digitalWrite(LED_ROUGE, HIGH);
  digitalWrite(LED_VERT,  LOW);
  digitalWrite(BUZZER,    LOW);
}

void afficherOuvert() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ACCES ACCORDE  ");
  lcd.setCursor(0, 1);
  lcd.print("  Bienvenue !   ");
  digitalWrite(LED_VERT,  HIGH);
  digitalWrite(LED_ROUGE, LOW);
  digitalWrite(BUZZER,    HIGH);
  delay(100);
  digitalWrite(BUZZER,    LOW);
}

void afficherRefus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ACCES REFUSE   ");
  lcd.setCursor(0, 1);
  lcd.print(" Code invalide  ");
  digitalWrite(LED_ROUGE, HIGH);
  digitalWrite(LED_VERT,  LOW);
  digitalWrite(BUZZER,    HIGH);
}

void afficherSaisie() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entrez le PIN:  ");
  lcd.setCursor(0, 1);
  for (int i = 0; i < saisieEnCours.length(); i++) {
    lcd.print("*");
  }
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(LED_VERT,  OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  afficherVerrouille();
}

void loop() {
  switch (etatActuel) {

    case VERROUILLE: {
      char touche = keypad.getKey();
      if (touche == '*') {
        saisieEnCours = "";
        etatActuel = SAISIE_PIN;
        afficherSaisie();
      }
      else if (touche == '#') {
        saisieEnCours = "";
        etatActuel = ADMIN;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode admin      ");
        lcd.setCursor(0, 1);
        lcd.print("Nouveau PIN:    ");
      }
      break;
    }

    case SAISIE_PIN: {
      char touche = keypad.getKey();
      if (touche) {
        if (touche == '*') {
          saisieEnCours = "";
          etatActuel = VERROUILLE;
          afficherVerrouille();
        }
        else if (saisieEnCours.length() < 4) {
          saisieEnCours += touche;
          afficherSaisie();

          if (saisieEnCours.length() == 4) {
            if (saisieEnCours == PIN_SECRET) {
              etatActuel = OUVERT;
              tempsChangementEtat = millis();
              afficherOuvert();
            } else {
              etatActuel = REFUS;
              tempsChangementEtat = millis();
              afficherRefus();
            }
            saisieEnCours = "";
          }
        }
      }
      break;
    }

    case OUVERT: {
      if (millis() - tempsChangementEtat >= DUREE_OUVERT) {
        etatActuel = VERROUILLE;
        afficherVerrouille();
      }
      break;
    }

    case REFUS: {
      if (millis() - tempsChangementEtat >= DUREE_REFUS) {
        digitalWrite(BUZZER, LOW);
        etatActuel = VERROUILLE;
        afficherVerrouille();
      }
      break;
    }

    case ADMIN: {
      char touche = keypad.getKey();
      if (touche) {
        if (touche == '*') {
          etatActuel = VERROUILLE;
          afficherVerrouille();
        }
        else if (saisieEnCours.length() < 4) {
          saisieEnCours += touche;
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          for (int i = 0; i < saisieEnCours.length(); i++) {
            lcd.print("*");
          }

          if (saisieEnCours.length() == 4) {
            PIN_SECRET = saisieEnCours;
            saisieEnCours = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("PIN modifie !   ");
            delay(2000);
            etatActuel = VERROUILLE;
            afficherVerrouille();
          }
        }
      }
      break;
    }
  }
}