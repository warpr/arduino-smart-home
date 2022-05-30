// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 12     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

//Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C the_screen(0x27, 16, 2);

uint8_t deg[8] = {0x7,0x5,0x7,0x0,0x0,0x0,0x0};
uint8_t deg_alt[8]  = {0x2,0x5,0x2,0x0,0x0,0x0,0x0};

// deg          deg_alt
// ..111 0x07   ...1. 0x02
// ..1.1 0x05   ..1.1 0x05
// ..111 0x07   ...1. 0x02
// ..... 0x00   ..... 0x00
// ..... 0x00   ..... 0x00
// ..... 0x00   ..... 0x00
// ..... 0x00   ..... 0x00

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

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

void setup() {
    Serial.begin(115200);
    Serial.println(F("Initializing Kuno's Arduino Smart Home!"));

    init_dht11();

    the_screen.init();
    the_screen.backlight();

    the_screen.createChar(0, deg);
    the_screen.createChar(1, deg_alt);

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

void loop() {
    // Wait a few milliseconds each iteration
    delay(200);

    update_dht11();

    dtostrf(current_temperature, 2, 1, display_buffer);

    the_screen.setCursor(0, 0);
    the_screen.print(display_buffer);
    the_screen.printByte(0);
    the_screen.print("C");
    the_screen.setCursor(0, 1);
/*
    strcpy(second_line,  "                ");

    pos = 0x0f & (pos + 1);
    second_line[pos] = '>';
    the_screen.print(second_line);
*/
}
