/*  Example sketch for the PCF8574 for the purposes of showing how to use the interrupt-pin.

    Attach the positive lead of an LED to PIN7 on the PCF8574 and the negative lead to GND,
    a wire from Arduino-pin 13 to pin 3 on the PCF8474, a wire from the int-pin on the PCF8574
    to Arduino-pin 7 and wires for SDA and SCL to Arduino-pins 2 and 3, respectively.

    If all goes well you should see the small blue LED on the ESP-module lighting up and the
    LED connected to the PCF going off, and vice versa. */

#include <pcf8574_esp.h>

/*  We need to set up the I2C-bus for the library to use */
#include <Wire.h>

// Initialize a PCF8574 at I2C-address 0x20
PCF857x pcf8574(0x20, &Wire);

//If you had a PCF8575 instead you'd use the below format
//PCF857x pcf8575(0x20, &Wire, true);

bool volatile PCFInterruptFlag = false;

void PCFInterrupt() {
  PCFInterruptFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  pinMode(13, OUTPUT);

  Wire.begin();
  //Specsheets say PCF8574 is officially rated only for 100KHz I2C-bus
  //PCF8575 is rated for 400KHz
  Wire.setClock(100000L);
  pcf8574.begin();
  // Most ready-made PCF8574-modules seem to lack an internal pullup-resistor, so you have to use the MCU-internal one.
  pinMode(2, INPUT_PULLUP);
  pcf8574.resetInterruptPin();
  pcf8574.write(7, HIGH);
  attachInterrupt(digitalPinToInterrupt(2), PCFInterrupt, FALLING);
}

void loop() {
  if(PCFInterruptFlag){
    pcf8574.rotateLeft(1);
    PCFInterruptFlag=false;
  }
  Serial.println(F("Blink."));
  if(digitalRead(13)==HIGH) digitalWrite(13, LOW);
  else digitalWrite(13, HIGH);
  delay(1000);
}
