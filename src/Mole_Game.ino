#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int NUM_MOLES = 4;
const int ledPins[NUM_MOLES] = {11, 12, 13, 10};
const int btnPins[NUM_MOLES] = {7, 8, 9, 6};
const int servoPins[NUM_MOLES] = {3, 4, 5, 2};
const int buzzer = A1;

Servo servos[NUM_MOLES];
int score = 0;

void setup() {
  for (int i = 0; i < NUM_MOLES; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(btnPins[i], INPUT_PULLUP);
    servos[i].attach(servoPins[i]);
    servos[i].write(0);
  }

  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready to play!");

  Serial.begin(9600);
  randomSeed(analogRead(0));

  for (int i = 3; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Start in ");
    lcd.print(i);
    lcd.print("... ");
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Start!");
  delay(1500);
  lcd.clear();
}

void playStage(int duration, int limitTimeMs, bool randomLED) {
  unsigned long startStage = millis();

  while (millis() - startStage < duration) {
    int moleIndex = random(0, NUM_MOLES);
    int ledIndex = moleIndex;

    if (randomLED) {
      ledIndex = random(0, NUM_MOLES);
      digitalWrite(ledPins[ledIndex], HIGH);
    }

    servos[moleIndex].write(90);

    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.print(score);
    lcd.setCursor(0, 1);
    lcd.print("Hit the mole!   ");

    Serial.print("두더지 ");
    Serial.print(moleIndex + 1);
    if (randomLED) {
      Serial.print(" (LED 방해: ");
      Serial.print(ledIndex + 1);
      Serial.print(")");
    }
    Serial.println();

    unsigned long appearTime = millis();
    bool answered = false;

    while (millis() - appearTime < limitTimeMs && !answered) {
      for (int i = 0; i < NUM_MOLES; i++) {
        if (digitalRead(btnPins[i]) == LOW) {
          if (i == moleIndex) {
            tone(buzzer, 1000, 300);
            score += 10;
            Serial.println("정답! +10점");
          } else {
            tone(buzzer, 500, 200);
            score -= 5;
            Serial.print("오답! 버튼 ");
            Serial.print(i + 1);
            Serial.println(" -5점");
          }
          answered = true;
          break;
        }
      }
    }

    servos[moleIndex].write(0);

    if (randomLED) {
      digitalWrite(ledPins[ledIndex], LOW);
    }

    delay(500);
    lcd.clear();
  }
}

void loop() {
  // Stage 1
  lcd.setCursor(0, 0);
  lcd.print("Stage 1");
  delay(1000);
  lcd.clear();
  playStage(20000, 3000, false);

  if (score < 20) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Need 20 points");
    lcd.setCursor(0, 1);
    lcd.print("Try again!");
    Serial.println("스테이지 1 실패. 20점 미만.");
    while (true);
  }

  // Stage 2
  lcd.setCursor(0, 0);
  lcd.print("Stage 2");
  delay(1000);
  lcd.clear();
  playStage(20000, 1500, false);

  if (score < 50) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Need 50 points");
    lcd.setCursor(0, 1);
    lcd.print("Try again!");
    Serial.println("스테이지 3 잠금. 50점 미만.");
    while (true);
  }

  // Stage 3
  lcd.setCursor(0, 0);
  lcd.print("Stage 3");
  delay(1000);
  lcd.clear();
  playStage(20000, 1500, true);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Final: ");
  lcd.print(score);
  Serial.print("게임 종료! 최종 점수: ");
  Serial.println(score);

  while (true);
}
