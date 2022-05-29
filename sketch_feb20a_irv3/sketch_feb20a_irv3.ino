#include <IRremote.hpp>

void setup(){
  // connect IR receiver to pin 7, see:
  // https://www.circuitbasics.com/arduino-ir-remote-receiver-tutorial/

  const int RECV_PIN = 7;
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(115200);
  
  for (int i = 0; i < 30; i++) {
    delay(1000);
    Serial.println("Booting...");
  }
  Serial.println("It starts!\n=========\n");
}

void loop(){
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial); // optional use new print version
      IrReceiver.resume(); // Enable receiving of the next value
  }
}