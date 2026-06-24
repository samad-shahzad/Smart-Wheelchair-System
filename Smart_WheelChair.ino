#include <SoftwareSerial.h>

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 9
#define ENB 10

#define joyX A0
#define joyY A1
#define joySW 7

#define TRIG 6
#define ECHO 8

int xValue, yValue, buttonState;
int rightSpeed, leftSpeed;
int currentRight = 0, currentLeft = 0;

long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joySW, INPUT_PULLUP);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);
}

void loop() {
  long d = getDistance();
  if (d < 30) {
    stopMotors();
    return;
  }

  xValue = analogRead(joyX);
  yValue = analogRead(joyY);
  buttonState = digitalRead(joySW);

  if (buttonState == LOW) {
    stopMotors();
    delay(200);
    return;
  }

  int moveValue = map(yValue, 0, 1023, -255, 255);
  int turnValue = map(xValue, 0, 1023, -255, 255);

  // Invert joystick directions
  moveValue = -moveValue;  // Forward becomes backward
  turnValue = -turnValue;  // Left becomes right

  if (abs(moveValue) < 50) moveValue = 0;
  if (abs(turnValue) < 50) turnValue = 0;

  // Swap move and turn to keep smooth control like before
  int temp = moveValue;
  moveValue = turnValue;
  turnValue = temp;

  rightSpeed = constrain(moveValue - turnValue, -255, 255);
  leftSpeed  = constrain(moveValue + turnValue, -255, 255);

  currentRight = smoothChange(currentRight, rightSpeed, 5);
  currentLeft  = smoothChange(currentLeft, leftSpeed, 5);

  setMotor(ENA, IN1, IN2, currentRight);
  setMotor(ENB, IN3, IN4, currentLeft);

  delay(30);
}

int smoothChange(int current, int target, int step) {
  if (current < target) current += step;
  else if (current > target) current -= step;
  return current;
}

void setMotor(int EN, int INa, int INb, int speed) {
  if (speed > 0) {
    digitalWrite(INa, HIGH);
    digitalWrite(INb, LOW);
  } else if (speed < 0) {
    digitalWrite(INa, LOW);
    digitalWrite(INb, HIGH);
  } else {
    digitalWrite(INa, LOW);
    digitalWrite(INb, LOW);
  }
  analogWrite(EN, abs(speed));
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
