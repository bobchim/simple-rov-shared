/*
Reduced ROV Main
Last Updated:             2025-04-10
Submit pull request to:   Bob - bobchim@protonmail.com
*/

// this code is written and tested on arduino mega pro

#include "CRC.h"
#include "motor.h"
#include "propulsion_system.h"

#define DEBUG true
#define Serial \
  if (DEBUG) Serial

CRC32 crc1;
CRC32 crc2;

float TXdata[0];
int8_t RXdata[7];
char delimiter = '\n';
uint32_t crc_rx, crc_rx_calc, crc_tx_calc;

static int rx_min = -128;
static int rx_max = 127;
static int pwm_min = 1000;
static int pwm_max = 2000;

unsigned long t0 = 0;
unsigned long t1 = 0;
unsigned long dt = 100;   // loop "delay" time
unsigned long ts = 2000;  // no signal wait time

// Set pin number for thrusters
Motor thrusters[] = {
  Motor(2, rx_min, rx_max, pwm_min, pwm_max, 1),  // vfl
  Motor(3, rx_min, rx_max, pwm_min, pwm_max, 1),  // vfr
  Motor(4, rx_min, rx_max, pwm_min, pwm_max, 1),  // vrl
  Motor(5, rx_min, rx_max, pwm_min, pwm_max, 1),  // vrr
  Motor(6, rx_min, rx_max, pwm_min, pwm_max, 1),  // hl
  Motor(7, rx_min, rx_max, pwm_min, pwm_max, 1),  // hr
  Motor(8, rx_min, rx_max, pwm_min, pwm_max, 1),  // hs
};

// Set pin number for servos
Motor servos[] = {
  Motor(9, rx_min, rx_max, pwm_min, pwm_max, 0),
};

// Define thrust vectors in rov frame for 45 degree motor config
// length of thrust vectors == number of thrusters
int x_dir[] = { 0, 0, 0, 0, 1, 1, 0 };
int y_dir[] = { 0, 0, 0, 0, 0, 0, -1 };
int z_dir[] = { 1, 1, 1, 1, 0, 0, 0 };
int roll_dir[] = { 1, -1, 1, -1, 0, 0, 0 };
int pitch_dir[] = { 1, 1, -1, -1, 0, 0, 0 };
int yaw_dir[] = { 0, 0, 0, 0, -1, 1, 0 };

PropulsionSystem propsys(7, thrusters, x_dir, y_dir, z_dir, roll_dir, pitch_dir, yaw_dir);

void setup() {
  Serial.begin(19200);  // USB
  Serial1.begin(19200);  // arduino to arduino

  // Attach pins
  propsys.Attach();
  servos[0].Attach();
}

void loop() {
  if (millis() - t0 >= dt) {
    t0 = millis();

    // Send data over serial with CRC
    crc1.restart();
    Serial1.write((char*)&delimiter, sizeof(char));
    for (int i = 0; i < sizeof(TXdata) / sizeof(float); ++i) {
      Serial1.write((const byte*)&TXdata[i], sizeof(float));
      crc1.add((byte*)&TXdata[i], sizeof(float));
    }
    crc_tx_calc = crc1.calc();
    Serial1.write((byte*)&crc_tx_calc, sizeof(uint32_t));

    // Receive data from serial with CRC validation
    crc2.restart();
    while (Serial1.available()) {
      char rx_start = Serial1.read();
      if (rx_start == delimiter) {
        for (int i = 0; i < sizeof(RXdata) / sizeof(byte); ++i) {
          Serial1.readBytes((byte*)&RXdata[i], sizeof(byte));
          crc2.add((byte*)&RXdata[i], sizeof(byte));
        }
      }
      crc_rx_calc = crc2.calc();
      Serial1.readBytes((byte*)&crc_rx, sizeof(uint32_t));
    }

    Serial.print("RX: ");
    Serial.print('\t');
    if (crc_rx_calc == crc_rx) {
      for (int i = 0; i < sizeof(RXdata) / sizeof(uint8_t); i++) {
        Serial.print(RXdata[i]);
        Serial.print('\t');
      }
      // Reset no signal timer
      t1 = millis();
    } else {
      Serial.print("Data Corrupted. ");
      Serial.print('\t');
    }

    // Stop motors if no signal for duration
    if (millis() - t1 >= ts) {
      Serial.print("Prop Disabled: ");
      Serial.print('\t');
      propsys.Reset();
    } else {
      Serial.print("Prop Enabled: ");
      Serial.print('\t');
      if (crc_rx_calc == crc_rx) {
        // Command propulsion system and servo system
        propsys.Drive(RXdata[1], RXdata[0], RXdata[3], 0, 0, RXdata[2]);
        servos[0].Spin(RXdata[4]);
      }
    }

    // Print thruster status from Motor instance
    for (int i = 0; i < 7; i++) {
      Serial.print(thrusters[i].getSpeed());
      Serial.print('\t');
    }
    Serial.println();
  }
}