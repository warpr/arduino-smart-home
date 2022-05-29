#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

#define DELAY_AFTER_SEND 2000
#define DELAY_AFTER_LOOP 5000

void setup(){
  const int RECV_PIN = 7;
  const int ir_send_pin = 3;

  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  Serial.begin(115200);

  delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!

  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  IrSender.begin(ir_send_pin, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin

  Serial.print(F("Ready to send IR signals at pin "));
  Serial.println(ir_send_pin);

#if !defined(SEND_PWM_BY_TIMER)
    /*
     * Print internal signal generation info
     */
    IrSender.enableIROut(38); // Call it with 38 kHz to initialize the values printed below
    Serial.print(F("Send signal mark duration is "));
    Serial.print(IrSender.periodOnTimeMicros);
    Serial.print(F(" us, pulse correction is "));
    Serial.print(IrSender.getPulseCorrectionNanos());
    Serial.print(F(" ns, total period is "));
    Serial.print(IrSender.periodTimeMicros);
    Serial.println(F(" us"));
#endif
}

/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint16_t sAddress = 0x04;
uint8_t sCommand = 0x03;
uint8_t sRepeats = 4;

void loop(){
  // connect IR receiver to pin 7, see:
  // https://www.circuitbasics.com/arduino-ir-remote-receiver-tutorial/

  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial); // optional use new print version
      IrReceiver.resume(); // Enable receiving of the next value
  }

  // for transmitting, see:
  // https://learn.sparkfun.com/tutorials/ir-communication/all

    /*
     * Print values
     */
    Serial.println();
    Serial.print(F("address=0x"));
    Serial.print(sAddress, HEX);
    Serial.print(F(" command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(" repeats="));
    Serial.println(sRepeats);
    Serial.println();
    Serial.println();
    Serial.flush();

    Serial.println(F("Send NEC with 8 bit address"));
    Serial.flush();
    IrSender.sendNEC(sAddress & 0xFF, sCommand, sRepeats);
    delay(DELAY_AFTER_SEND); // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal

    Serial.println(F("Send NEC with 16 bit address"));
    Serial.flush();
    IrSender.sendNEC(sAddress, sCommand, sRepeats);
    delay(DELAY_AFTER_SEND);

    Serial.println(F("Send LG"));
    Serial.flush();
    IrSender.sendLG(sAddress, sCommand, sRepeats);
    delay(DELAY_AFTER_SEND);

    Serial.flush();
    Serial.println(F("Send NEC with 16 bit address 0x0102 and command 0x34 with old 32 bit format MSB first"));
    IrSender.sendNECMSB(0xFC03FB04, 32, false);
    delay(DELAY_AFTER_SEND);

    IRData IRSendData;
    IRSendData.address = sAddress;
    IRSendData.command = sCommand;
    IRSendData.flags = IRDATA_FLAGS_EMPTY;
    
    IRSendData.protocol = LG;
    Serial.print(F("Send with IrSender.write - "));
    Serial.println(getProtocolString(IRSendData.protocol));
    Serial.flush();
    IrSender.write(&IRSendData, sRepeats);
    delay(DELAY_AFTER_SEND);

/*
    IRSendData.protocol = LG;
    Serial.print(F("Send "));
    Serial.println(getProtocolString(IRSendData.protocol));
    Serial.flush();
    IrSender.write(&IRSendData, sRepeats);
    delay(DELAY_AFTER_SEND);
*/
    delay(DELAY_AFTER_LOOP); // additional delay at the end of each loop
}
