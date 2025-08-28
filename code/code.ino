#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SensirionI2CSen5x.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C  // Usually 0x3C for 128x64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SensirionI2CSen5x sen5x;

#define MOTOR_PIN 3   // Use PWM capable pin (3, 5, 6, 9, 10, 11 on Nano)

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(MOTOR_PIN, OUTPUT);

  // Init display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // Init sensor
  sen5x.begin(Wire);
  uint16_t error;

  error = sen5x.deviceReset();
  if (error) {
    Serial.println("Reset failed!");
  }

  error = sen5x.startMeasurement();
  if (error) {
    Serial.println("Start measurement failed!");
  }

  delay(2000);
}

void loop() {
  uint16_t error;

  // Variables for PM readings
  float mc_1p0, mc_2p5, mc_4p0, mc_10p0;
  float nc_0p5, nc_1p0, nc_2p5, nc_4p0, nc_10p0;
  float typical_particle_size;

  error = sen5x.readMeasuredPmValues(
              mc_1p0, mc_2p5, mc_4p0, mc_10p0,
              nc_0p5, nc_1p0, nc_2p5, nc_4p0, nc_10p0,
              typical_particle_size);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (!error) {
    // Show PM2.5
    Serial.print("PM2.5: ");
    Serial.println(mc_2p5, 1);

    display.print("PM2.5: ");
    display.print(mc_2p5, 1);
    display.println(" ug/m3");

    // Control motor based on PM2.5
    int pwmValue = 0;
    if (mc_2p5 <= 50) {
      pwmValue = 100;
    } else if (mc_2p5 <= 100) {
      pwmValue = 200;
    } else {
      pwmValue = 255;
    }

    analogWrite(MOTOR_PIN, pwmValue);

    display.print("Fan PWM: ");
    display.println(pwmValue);

  } else {
    Serial.println("Read error!");
    display.println("Sensor Error");
    analogWrite(MOTOR_PIN, 0); // stop motor if error
  }

  display.display();
  delay(1000);
}
