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

## Circuit Diagram
<img width="2510" height="1846" alt="Image" src="https://github.com/user-attachments/assets/49375fba-0313-42cc-9949-37971629fafe" />

## Software and Package Required
**Arduino IDE**  
The libraries required are listed below  
- **Wire.h** (Arduino Core Component)
- **Adafruit_GFX.h** [Link](https://github.com/adafruit/Adafruit-GFX-Library)  
- **Adafruit_SSD1306.h** For controlling OLED Display [Link](https://github.com/adafruit/Adafruit_SSD1306)  
- **SensirionI2CSen5x.h** For controlling Sensirion Sen50 sensor [Link](https://github.com/Sensirion/arduino-i2c-sen5x)  

## Code
```
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
While installing, make sure the top cover matches the boles created for the SEN50 sensor and the landing legs on the cover is matching with holes on the middle part. Lastly, the bottom HEPA filter cover should be added to the middle part by the M2 screw through the allocated holes.
