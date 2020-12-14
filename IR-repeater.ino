/*
 * IRrecord: record and play back IR signals as a minimal
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR LED must be connected to the output PWM pin 3.
 * A button must be connected between the input SEND_BUTTON_PIN and ground.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * Initially coded 2009 Ken Shirriff http://www.righto.com
 */

#include <IRremote.h>

#if defined(ESP32)
int IR_RECEIVE_PIN = 15;
int SEND_BUTTON_PIN = 16; // RX2 pin
#else
int IR_RECEIVE_PIN = 11;
int SEND_BUTTON_PIN = 12;
#endif
int STATUS_PIN = LED_BUILTIN;

int DELAY_BETWEEN_REPEAT = 30;

// LG uses SAMSUNG protocol (32 bits)
#define LG_BITS             32
#define LG_POWER            0x34347887
#define LG_VOL_DOWN         0x34346897
#define LG_VOL_UP           0x3434E817
#define LG_MUTE             0x3434F807
#define LG_INPUT_OPTICAL    0x34346D92
#define LG_SOUND_3D         0x343409F6
#define LG_INFO             0x3434C53A
#define LG_UP               0x3434E51A
#define LG_DOWN             0x3434659A
#define LG_LEFT             0x343415EA
#define LG_RIGHT            0x3434956A
#define LG_OK               0x343455AA
#define LG_BACK             0x343445BA

// PHILIPS uses RC6 protocol (20 bits)
#define PHILIPS_BITS            20
#define PHILIPS_POWER           0xC
#define PHILIPS_POWER_REPEAT    0x1000C
#define PHILIPS_VOL_UP          0x10
#define PHILIPS_VOL_UP_REPEAT   0x10010
#define PHILIPS_VOL_DOWN        0x11
#define PHILIPS_VOL_DOWN_REPEAT 0x10011
#define PHILIPS_MUTE            0xD
#define PHILIPS_MUTE_REPEAT     0x1000D
#define PHILIPS_RED             0x6D
#define PHILIPS_RED_REPEAT      0x1006D
//#define PHILIPS_GREEN           0x6E
//#define PHILIPS_GREEN_REPEAT    0x1006E
//#define PHILIPS_YELLOW          0x6F
//#define PHILIPS_YELLOW_REPEAT   0x1006F
#define PHILIPS_UP              0x58
#define PHILIPS_UP_REPEAT       0x10058
#define PHILIPS_DOWN            0x59
#define PHILIPS_DOWN_REPEAT     0x10059
#define PHILIPS_LEFT            0x5A
#define PHILIPS_LEFT_REPEAT     0x1005A
#define PHILIPS_RIGHT           0x5B
#define PHILIPS_RIGHT_REPEAT    0x1005B
#define PHILIPS_RECORD          0x37
#define PHILIPS_RECORD_REPEAT   0x10037
//#define PHILIPS_BACK            0xA
//#define PHILIPS_BACK_REPEAT     0x1000A
#define PHILIPS_TEXT            0x3C
#define PHILIPS_TEXT_REPEAT     0x1003C


IRrecv IrReceiver(IR_RECEIVE_PIN);
IRsend IrSender;

// On the Zero and others we switch explicitly to SerialUSB
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

void setup() {
    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__));

    IrReceiver.enableIRIn();  // Start the receiver
    IrReceiver.blink13(true); // Enable feedback LED

    pinMode(STATUS_PIN, OUTPUT);

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);

#if defined(SENDING_SUPPORTED)
    Serial.print(F("Ready to send IR signals at pin "));
    Serial.print(IR_SEND_PIN);
    Serial.print(F(" on press of button at pin "));
    Serial.println(SEND_BUTTON_PIN);

#else
    Serial.println(F("Sending not supported for this board!"));
#endif
    Serial.println();
}

// Storage for the recorded code
int codeType = -1; // The type of code
uint32_t codeValue; // The code value if not raw
uint8_t codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
void storeCode() {
    if (IrReceiver.results.isRepeat) {
        Serial.println("Ignore repeat");
        return;
    }

    codeType = IrReceiver.results.decode_type;
    codeValue = IrReceiver.results.value;
    codeLen = IrReceiver.results.bits;
    
    if (codeType == RC6) {
        Serial.println("Received RC6 code:");
        IrReceiver.printResultShort(&Serial);
        Serial.println("Converting Philips code to LG code.. ");
        Serial.print(codeValue);
        Serial.print(" becomes ");
        codeValue = philipsToLG(codeValue);
        codeLen = LG_BITS;
        Serial.print(codeValue);
        Serial.println();
        Serial.println();
    } else if (codeType == SAMSUNG) {
        Serial.println("Received SAMSUNG code:");
        IrReceiver.printResultShort(&Serial);
        Serial.println();
    } else {
        Serial.println("Received unknown code, discarding data..");
        Serial.println();
        codeValue = 0;
        codeLen = 0;
    }
}

void sendCode() {
  if (codeLen > 0 && codeValue > 0) {
      Serial.println("Sending SAMSUNG/LG: ");
      Serial.println(codeValue, HEX);
      IrSender.sendSAMSUNG(codeValue, codeLen);
      Serial.println();
      codeValue = 0;
      codeLen = 0;
    }
}

uint32_t philipsToLG(uint32_t philipsCodeValue) {
  switch (philipsCodeValue) {
    case PHILIPS_VOL_UP:
    case PHILIPS_VOL_UP_REPEAT:
      return LG_VOL_UP;
      
    case PHILIPS_VOL_DOWN:
    case PHILIPS_VOL_DOWN_REPEAT:
      return LG_VOL_DOWN;
      
    case PHILIPS_MUTE:
    case PHILIPS_MUTE_REPEAT:
      return LG_MUTE;
      
    case PHILIPS_RED:
    case PHILIPS_RED_REPEAT:
      return LG_POWER;
//      
//    case PHILIPS_GREEN:
//    case PHILIPS_GREEN_REPEAT:
//      return LG_INPUT_OPTICAL;
//      
//    case PHILIPS_YELLOW:
//    case PHILIPS_YELLOW_REPEAT:
//      return LG_SOUND_3D;
      
    case PHILIPS_UP:
    case PHILIPS_UP_REPEAT:
      return LG_UP;
      
    case PHILIPS_DOWN:
    case PHILIPS_DOWN_REPEAT:
      return LG_DOWN;
      
    case PHILIPS_LEFT:
    case PHILIPS_LEFT_REPEAT:
      return LG_LEFT;
      
    case PHILIPS_RIGHT:
    case PHILIPS_RIGHT_REPEAT:
      return LG_RIGHT;
      
    case PHILIPS_RECORD:
    case PHILIPS_RECORD_REPEAT:
      return LG_OK;
      
//    case PHILIPS_BACK:
//    case PHILIPS_BACK_REPEAT:
//      return LG_BACK;

    case PHILIPS_TEXT:
    case PHILIPS_TEXT_REPEAT:
      return LG_INPUT_OPTICAL;

    default: 
      return 0;
  }
}

void loop() {
    if (IrReceiver.decode()) {
        digitalWrite(STATUS_PIN, HIGH);
        storeCode();
        IrReceiver.resume(); // resume receiver
        sendCode();
        digitalWrite(STATUS_PIN, LOW);
        delay(DELAY_BETWEEN_REPEAT); // Wait a bit between retransmissions
        IrReceiver.enableIRIn(); // Re-enable receiver
    }
}
