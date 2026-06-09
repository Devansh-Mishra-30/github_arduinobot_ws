#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(9);
  delay(500);

  myservo.write(10);
  delay(1000);      // give servo time to reach 90

  myservo.detach(); // stop sending signal
}

void loop() {
}
