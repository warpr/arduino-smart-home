#include <Servo.h>
Servo servo_10;

void setup(){
  Serial.begin(115200);
  servo_10.attach(10);
}
void loop(){
  Serial.println("Write 90");
  servo_10.write(90);
  delay(1000);
  Serial.println("Write -90");
  servo_10.write(-90);
  delay(1000);
}