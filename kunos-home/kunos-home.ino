// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <LiquidCrystal_I2C.h>
#include "DHT.h"

/**
 * MQ-2 Gas Sensor
 */
volatile int gas;
volatile int photocell;
volatile int motion;

void init_sensors() {
    pinMode(A0, INPUT); // set A0 to input (MQ-2 Gas Sensor)
    pinMode(A1, INPUT); // set A1 to input (Photocell)
    pinMode(2, INPUT);  // set digital 2 to input (PIR Motion Sensor)
}

void update_sensors() {
    gas = analogRead(A0);
    photocell = analogRead(A1);
    motion = digitalRead(2);

    if (motion == 1) {
        digitalWrite(5, HIGH); // yellow led on
    } else {
        digitalWrite(5, LOW);  // yellow led off
    }

    if (photocell > 300) {
        digitalWrite(13, HIGH); // white led on
    } else {
        digitalWrite(13, LOW);  // white led off
    }
}

/**
 * DHT-11 Temperature and Humidity sensor
 * ======================================
 */

#define DHTPIN 12     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

unsigned long last_dht11_at;
float current_temperature;
float current_humidity;

void init_dht11() {
    last_dht11_at = millis();

    dht.begin();
}

void update_dht11() {
    unsigned long elapsed = millis() - last_dht11_at;

    if (elapsed < 2000) {
        return;
    }

    last_dht11_at = millis();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed
    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
    } else {
        current_humidity = h;
        current_temperature = t;

        Serial.print(F("[Updated] Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.println(F("°C "));
    }
}

/**
 * LCD Display
 * ===========
 */

//Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C the_screen(0x27, 16, 2);

uint8_t deg[8] = {0x7,0x5,0x7,0x0,0x0,0x0,0x0};
uint8_t deg_alt[8]  = {0x2,0x5,0x2,0x0,0x0,0x0,0x0};
uint8_t drop[8] = {0x0,0x4,0xe,0xe,0x1f,0xe,0x0};

// deg          deg_alt      drop
// ..111 0x07   ...1. 0x02   ..... 0x00
// ..1.1 0x05   ..1.1 0x05   ..1.. 0x04
// ..111 0x07   ...1. 0x02   .111. 0x0e
// ..... 0x00   ..... 0x00   .111. 0x0e
// ..... 0x00   ..... 0x00   11111 0x1f
// ..... 0x00   ..... 0x00   .111. 0x0e
// ..... 0x00   ..... 0x00   ..... 0x00

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

void init_display() {
    the_screen.init();
    the_screen.backlight();

    the_screen.createChar(0, deg);
    the_screen.createChar(1, deg_alt);
    the_screen.createChar(2, drop);

    the_screen.setCursor(0, 0);
    the_screen.print("Kuno's Arduino");
    the_screen.setCursor(0, 1);
    the_screen.print("Smart Home!");

    delay(200);
    the_screen.clear();
}

char empty_line = "                ";
char display_buffer[17];
char second_line[17];

void update_display() {
    dtostrf(current_temperature, 2, 1, display_buffer);

    the_screen.setCursor(0, 0);
    the_screen.print(display_buffer);
    the_screen.printByte(0);
    the_screen.print("C ");
    dtostrf(current_humidity, 3, 0, display_buffer);
    the_screen.print(display_buffer);
    the_screen.print("% ");
    the_screen.printByte(2);
    the_screen.setCursor(0, 1);

    sprintf(second_line, "%d ppm", gas);
    the_screen.print(second_line);
}

/**
 * Kuno's Arduino Smart Home setup
 * ===============================
 */

void setup() {
    Serial.begin(115200);
    Serial.println(F("Initializing Kuno's Arduino Smart Home!"));

    init_dht11();
    init_display();
    init_sensors();
}

/**
 * Kuno's Arduino Smart Home main loop
 * ===================================
 */

void loop() {
    // Wait a few milliseconds each iteration
    delay(200);

    update_dht11();
    update_display();
    update_sensors();
}
