#include <Wire.h>
#include <U8g2lib.h>
#include <SensirionI2CSen5x.h>

//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);      //Uncomment for 1.3 OLED
U8G2_SSD1306_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);       //Uncomment for .96 OLED

SensirionI2CSen5x sen5x;

#define MOTOR_PIN 3

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(MOTOR_PIN, OUTPUT);
  u8g2.begin();

  sen5x.begin(Wire);
  uint16_t error;

  error = sen5x.deviceReset();
  Serial.print("Reset: "); Serial.println(error);

  error = sen5x.startMeasurement();
  Serial.print("Start: "); Serial.println(error);

  delay(2000);
}

void loop() {
  uint16_t error;
  float mc_1p0, mc_2p5, mc_4p0, mc_10p0;
  float temp, humidity, voc, nox;

  error = sen5x.readMeasuredValues(
              mc_1p0, mc_2p5, mc_4p0, mc_10p0,
              humidity, temp, voc, nox);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  if (!error) {
    u8g2.setCursor(0, 12);
    u8g2.print("PM2.5: ");
    u8g2.print(mc_2p5, 1);

    u8g2.setCursor(0, 28);
    u8g2.print("Temp: ");
    u8g2.print(temp, 1);

    u8g2.setCursor(0, 44);
    u8g2.print("RH: ");
    u8g2.print(humidity, 1);
    u8g2.print(" %");

    int pwmValue = 0;
    if (mc_2p5 <= 30) {
      pwmValue = 100;
    } else if (mc_2p5 <= 100) {
      pwmValue = 200;
    } else {
      pwmValue = 255;
    }
    
    analogWrite(MOTOR_PIN, pwmValue);
    float fanspeed=(pwmValue/255.0)*100.0;
    u8g2.setCursor(0, 60);
    u8g2.print("Fan Speed: ");
    u8g2.print(fanspeed,1);
    u8g2.print(" %");

  } else {
    u8g2.setCursor(0, 30);
    u8g2.print("Sensor Err ");
    u8g2.print(error);
    analogWrite(MOTOR_PIN, 0);
  }
  
  drawSmiley(mc_2p5);
  u8g2.sendBuffer();
  delay(1000);
}

void drawSmiley(int pm25) {
  int x = 100;   // X position (top-right)
  int y = 20;    // Y position
  int r = 16;    // face radius

  // Face outline
  u8g2.drawCircle(x, y, r, U8G2_DRAW_ALL);

  // Eyes
  u8g2.drawDisc(x-6, y-5, 2);
  u8g2.drawDisc(x+6, y-5, 2);

  // Mouth based on PM2.5
if (pm25 <= 30) {
  // 😀 big smile
  u8g2.drawArc(x, y+3, 6, 125, 260);
} else if (pm25 <= 60) {
  // 🙂 small smile
  u8g2.drawArc(x, y+3, 6, 140, 240);
} else if (pm25 <= 90) {
  // 😐 flat
  u8g2.drawLine(x-6, y+5, x+6, y+5);
} else if (pm25 <= 120) {
  // 🙁 small frown
  u8g2.drawArc(x, y+8, 6, 280, 100);
} else if (pm25 <= 250) {
  // 😣 big frown
  u8g2.drawArc(x, y+8, 6, 250, 130);
} else {
  // 😷 mask
  u8g2.drawBox(x-8, y+2, 16, 10);
  u8g2.drawLine(x-8, y+2, x-14, y);
  u8g2.drawLine(x+8, y+2, x+14, y);
}

}


