# DIY Smart Air Purifier
A low-cost **DIY Smart Air Purifier** that automatically adjusts fan speed based on real-time **PM<sub>2.5</sub>** concentration. The purifier uses a **HEPA filter** with a DC fan mounted on top and is designed to provide adaptive air cleaning for indoor environments.

This project includes both the **hardware setup** and **Arduino-based control code**, and is intended to be fully open-source for anyone to replicate or improve.

## Features
**Real-Time Monitoring** – Continuously monitors air quality using a PM<sub>2.5</sub> sensor.  
**Automatic Fan Control** – Adjusts fan speed dynamically according to PM<sub>2.5</sub> levels.  
**Energy Efficient** – Runs at lower speeds when air quality is good, saving power.  
**HEPA Filtration** – Effective particle removal for healthier indoor air.  
**Open Source** – Hardware design, circuit, and code freely available.  

## Hardware Requirements
**PM<sub>2.5</sub> Sensor** - Sensirion's SEN50 [Link](https://in.element14.com/sensirion/sen50-sdn-t/particle-sensor-dust-digital-5v/dp/3953034?srsltid=AfmBOorvVw0ERm18eGjQajDrGZsDv7g6_JHfXTA9BqazueTBrIli2FY9)  
**HEPA Filter** - 20cm diameter, 29cm height(Standard MI air purifier Filter) [Link](https://amzn.in/d/hq0hpHi)  
**Power Supply** - 12V 2amp DC power supply [Link](https://robu.in/product/standard-12v-2a-power-supply-with-5-5mm-dc-plug-3-pin-input-plug/?gad_source=1&gad_campaignid=17427802559&gbraid=0AAAAADvLFWcdk5uJLfCHM6De_ulopqTzH&gclid=Cj0KCQjw_L_FBhDmARIsAItqgt7zm-Y9qGKT5erz5DKq4dK9PpbFHKkZJMZmfQivdFSZSkS9uHrLNrsaAgeSEALw_wcB)  
**DC/AC Fan** - 120mm*120mm, 12V DC Fan [Link](https://amzn.in/d/9E89Q2z)  
**Microcontroller** - Arduino Nano [Link](https://in.element14.com/arduino/a000005/arduino-nano-evaluation-board/dp/1848691)  
**Motor Driver** - L298N Motor Driver [Link](https://in.element14.com/dfrobot/dri0002/dc-motor-controller-mdv-2x2a-board/dp/3517873)  
**Display** - 0.96inch OLED display [Link](https://in.element14.com/midas/mdob128064wv-ybi/graphic-oled-cob-128-x-64pixel/dp/3565084)  

## PM sensor  
The current design supports **three Sensirion models**: **SEN50, SEN54, and SEN55**.  
All of them share the same physical footprint, making them interchangeable in this setup.  

- **SEN50** → Reports only **particulate matter (PM) values**, which are sufficient for the current application.  
- **SEN54 / SEN55** → Provide additional measurements such as **temperature, humidity, VOC, and NO<sub>x</sub>**, but come at a higher cost.  

👉 You can choose the sensor model based on the required parameters and budget.

## .9 inch vs 1.3 inch OLED Display  
**Current design files support both `0.96-inch` and `1.3-inch` OLED displays.**  
Please note that the pin locations for **`VCC`** and **`GND`** may vary depending on the display module, so always check the OLED **pinout** before connecting it to the circuit.
<img width="2192" height="745" alt="Image" src="https://github.com/user-attachments/assets/c054efee-0357-4398-a1f2-4a2fd0caddaf" />  

## Circuit Diagram for **2 Pin 12v DC Fan**
Both **2-pin** and **3-pin DC fans** can be used in this project.  
For controlling a 2-pin DC fan, we used an **L298N motor driver module** since it was readily available. Other methods, like MOSFET transistors, can also be used.

<img width="2510" height="1846" alt="Image" src="https://github.com/user-attachments/assets/49375fba-0313-42cc-9949-37971629fafe" />  

### Assembly  
<img width="1690" height="812" alt="Image" src="https://github.com/user-attachments/assets/821a62e2-41ba-4848-b545-61cc15a72f58" />

## Circuit Diagram for **3 Pin PWM 12v DC Fan**  
For **3-pin PWM fans**, the **12V supply** can be connected directly to the fan while the **PWM control signal** is driven from the Arduino Nano.  
This approach simplifies the circuit design since no external motor driver is required.  

⚠️ However, PWM fans are generally **more expensive** than standard 2-pin fans, which can increase the overall cost of the project.

<img width="2365" height="2153" alt="Image" src="https://github.com/user-attachments/assets/4a9639f9-ba3d-4d45-8d29-1d19b0e6c5d8" />  

## Software and Package Required
**Arduino IDE**  
The libraries required are listed below  
- **Wire.h** (Arduino Core Component)
- **SensirionI2CSen5x.h** For controlling Sensirion Sen50 sensor [Link](https://github.com/Sensirion/arduino-i2c-sen5x)  
- **U8g2lib.h** For controlling the OLED Display [Link](https://github.com/olikraus/u8g2)
  
## Code
```
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

```
## Setup Instruction
**Hardware Setup Instruction**
- For a beginner, I recommend testing each component one by one before assembling the entire circuit.
- Pull-up resistor is compulsory in Arduino nano for I2C communication [Video](https://www.youtube.com/watch?v=Z1_Ag3dxH34&t=75s), [Website](https://www.totalphase.com/blog/2024/07/what-are-i2c-pull-up-resistors-calculate-their-values/?srsltid=AfmBOooVGtZDZ16jqQqMrEwgEsxJL0CeHHH9oVToXDv3ci8wbzIYAD7i)
- For the entire circuit, including the 12V DC motor, power is provided by a 12V DC adapter. Please be careful with the connection. Don't fry the nano.
- Test the entire circuit before going for the final assembly.

## 3D Printed Parts and Assembly Guidelines
**3D Parts**  
There are 3 parts in the model. **Top(Cover)**, **Middle(Hardware mounting)**, and the **Bottom part(Hepa Mount)**
<img width="1719" height="1553" alt="Image" src="https://github.com/user-attachments/assets/3022ccb8-ffc7-43e0-89d1-9ccf57ec33a6" />

The top cover includes a slot for the OLED display, which can be secured on the designated pads using M2 screws (1–2 screws are sufficient). The other components—such as the SEN50 PM<sub>2.5</sub> sensor, L298N motor driver, Arduino Nano, and fan—should be installed at the positions indicated below.  
![Image](https://github.com/user-attachments/assets/368e49c1-a0a3-4169-99fc-fa099cbbc6c5)
While installing, make sure the top cover matches the holes created for the SEN50 sensor and the landing legs on the cover match with holes on the middle part. Lastly, the bottom HEPA filter cover should be added to the middle part by the M2 screw through the allocated holes.

<img width="210" height="112" alt="oshw_facts" src="https://github.com/user-attachments/assets/33640976-31aa-472c-972d-a6f8a911e5b4" />

Guess what? This project is certified by Open Source Hardware Association 🙂
<img width="590" height="165" alt="image" src="https://github.com/user-attachments/assets/b39780db-4f53-4ac9-806e-eeeb45b641ec" />

