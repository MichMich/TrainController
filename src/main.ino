#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>


#define CW_PIN 2
#define CCW_PIN 3
#define BUTTON_PIN 4

#define MOTOR_A_ENABLE 12
#define MOTOR_A_FWD 11
#define MOTOR_A_REV 6

#define MOTOR_B_ENABLE 8
#define MOTOR_B_FWD 9
#define MOTOR_B_REV 5


#define LONG_PRESS 1000

volatile int motorASpeed = 0;
volatile int motorBSpeed = 0;

bool toggle = false;
bool toggled = false;
bool lastButtonState = false;
long buttonPressedTime = 0;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void rotate() {
  if (toggle) {
    if (digitalRead(CCW_PIN)) {
      motorASpeed++;
    } else {
      motorASpeed--;
    }

    motorASpeed = constrain(motorASpeed, -100, 100);

  } else {
    if (digitalRead(CCW_PIN)) {
      motorBSpeed++;
    } else {
      motorBSpeed--;
    }

    motorBSpeed = constrain(motorBSpeed, -100, 100);

  }
}

void setup()
{
  u8g2.begin();

  pinMode(CW_PIN, INPUT_PULLUP);
  pinMode(CCW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(MOTOR_A_ENABLE, OUTPUT);
  pinMode(MOTOR_A_FWD, OUTPUT);
  pinMode(MOTOR_A_REV, OUTPUT);

  attachInterrupt(0, rotate, RISING);
  // attachInterrupt(1, rotate, RISING);
}

void loop()
{
  u8g2.firstPage();
  do {


    // Button

    if (lastButtonState != digitalRead(BUTTON_PIN)) {
      if (!digitalRead(BUTTON_PIN)) {
        buttonPressedTime = millis();
      } else {
        if (millis() - buttonPressedTime < LONG_PRESS) {
          motorASpeed = 0;
          motorBSpeed = 0;
        }
      }
      lastButtonState = digitalRead(BUTTON_PIN);
    }

    if (!digitalRead(BUTTON_PIN)) {
      if (millis() - buttonPressedTime > LONG_PRESS && !toggled) {
        toggle = !toggle;
        toggled = true;
      }
    } else {
      toggled = false;
    }

    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.setCursor(0, 16);
    u8g2.print(toggle ? "MOTOR A" : "MOTOR B");

    // Rotary
    u8g2.setFont(u8g2_font_logisoso26_tr);


    char speed[4];
    sprintf(speed, "%d", abs((toggle) ? motorASpeed : motorBSpeed));
    int speedWidth = u8g2.getUTF8Width(speed);
    u8g2.setCursor(72 - speedWidth, 63);
    u8g2.print(speed);

    u8g2.setCursor(70, 53);
    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.print(" km/h");


    //Draw Speed Bar
    u8g2.drawFrame(0, 22, 128, 10);
    u8g2.drawBox(0, 22, 128*abs(motorASpeed)/100, 10);

    //Draw Direction

    if (((toggle) ? motorASpeed : motorBSpeed) > 0) {
      u8g2.drawTriangle(0,40, 10,50, 0,60);
      u8g2.drawTriangle(11,40, 21,50, 11,60);
    } else if (((toggle) ? motorASpeed : motorBSpeed) < 0) {
      u8g2.drawTriangle(10,40, 0,50, 10,60);
      u8g2.drawTriangle(21,40, 11,50, 21,60);
    }

    // Draw Wissel
    u8g2.drawLine(97, 0, 105, 0);
    u8g2.drawLine(97, 1, 105, 1);
    u8g2.drawLine(97, 2, 105, 2);

    u8g2.drawLine(97, 14, 105, 14);
    u8g2.drawLine(97, 15, 105, 15);
    u8g2.drawLine(97, 16, 105, 16);

    u8g2.drawLine(119, 14, 127, 14);
    u8g2.drawLine(119, 15, 127, 15);
    u8g2.drawLine(119, 16, 127, 16);

    if (!toggle) {
      u8g2.drawLine(105, 14, 119, 14);
      u8g2.drawLine(105, 15, 119, 15);
      u8g2.drawLine(105, 16, 119, 16);
    } else {
      u8g2.drawLine(105, 0, 119, 14);
      u8g2.drawLine(105, 1, 119, 15);
      u8g2.drawLine(105, 2, 119, 16);
    }

  } while ( u8g2.nextPage() );


  //set motor speeds
  if (motorASpeed > 0) {
    digitalWrite(MOTOR_A_ENABLE, true);
    analogWrite(MOTOR_A_FWD, 0);
    analogWrite(MOTOR_A_REV, map(abs(motorASpeed), 0, 100, 0, 255));
  } else if (motorASpeed < 0) {
    digitalWrite(MOTOR_A_ENABLE, true);
    analogWrite(MOTOR_A_FWD, map(abs(motorASpeed), 0, 100, 0, 255));
    analogWrite(MOTOR_A_REV, 0);
  } else {
    digitalWrite(MOTOR_A_ENABLE, false);
    analogWrite(MOTOR_A_FWD, 0);
    analogWrite(MOTOR_A_REV, 0);
  }

  if (motorBSpeed > 0) {
    digitalWrite(MOTOR_B_ENABLE, true);
    analogWrite(MOTOR_B_FWD, 0);
    analogWrite(MOTOR_B_REV, map(abs(motorBSpeed), 0, 100, 0, 255));
  } else if (motorBSpeed < 0) {
    digitalWrite(MOTOR_B_ENABLE, true);
    analogWrite(MOTOR_B_FWD, map(abs(motorBSpeed), 0, 100, 0, 255));
    analogWrite(MOTOR_B_REV, 0);
  } else {
    digitalWrite(MOTOR_B_ENABLE, false);
    analogWrite(MOTOR_B_FWD, 0);
    analogWrite(MOTOR_B_REV, 0);
  }

  delay(25);
}
