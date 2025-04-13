/*
Reduced ROV Controller
Last Updated:             2025-04-10
Submit pull request to:   Bob - bobchim@protonmail.com
*/

// this code is written and tested on arduino mega pro.

#include "CRC.h"

#define DEBUG true
#define Serial \
  if (DEBUG) Serial

CRC32 crc1;
CRC32 crc2;

int8_t TXdata[7];
float RXdata[0];
char delimiter = '\n';
uint32_t crc_rx, crc_rx_calc, crc_tx_calc;

static int tx_min = -128;
static int tx_max = 127;
static int joystick_min = 0;
static int joystick_max = 1023;

unsigned long t0 = 0;
unsigned long dt = 100;   // loop "delay" time

void setup() {
  Serial.begin(115200);  // USB
  Serial3.begin(57600);  // arduino to arduino
}

void loop() {
  if (millis() - t0 >= dt) {
    t0 = millis();

    // Joystick
    TXdata[0] = joystick_response(2, joystick_min, joystick_max, tx_min, tx_max, 10);  // x, forward positive
    TXdata[1] = joystick_response(3, joystick_min, joystick_max, tx_min, tx_max, 10);  // y, right positive
    TXdata[2] = joystick_response(4, joystick_min, joystick_max, tx_min, tx_max, 10);  // z, up positive
    TXdata[3] = joystick_response(5, joystick_min, joystick_max, tx_min, tx_max, 10);  // roll
    TXdata[4] = joystick_response(6, joystick_min, joystick_max, tx_min, tx_max, 10);  // pitch
    TXdata[5] = joystick_response(7, joystick_min, joystick_max, tx_min, tx_max, 10);  // yaw
    TXdata[6] = joystick_response(8, joystick_min, joystick_max, tx_min, tx_max, 0);   // claw

    // Preview TX data
    Serial.print("TX: ");
    Serial.print('\t');
    for (int i = 0; i < sizeof(TXdata) / sizeof(byte); i++) {
      Serial.print(TXdata[i]);
      Serial.print('\t');
    }

    // Send data over serial with CRC
    crc2.restart();
    crc2.add((byte*)&TXdata, sizeof(TXdata));
    crc_tx_calc = crc2.calc();
    Serial3.write((char*)&delimiter, sizeof(char));
    Serial3.write((byte*)&TXdata, sizeof(TXdata));
    Serial3.write((byte*)&crc_tx_calc, sizeof(uint32_t));

    // Receive data from serial with CRC validation
    crc1.restart();
    while (Serial3.available()) {
      char rx_start = Serial3.read();
      if (rx_start == delimiter) {
        for (int i = 0; i < sizeof(RXdata) / sizeof(float); ++i) {
          Serial3.readBytes((byte*)&RXdata[i], sizeof(float));
          crc1.add((byte*)&RXdata[i], sizeof(float));
        }
        crc_rx_calc = crc1.calc();
        Serial3.readBytes((byte*)&crc_rx, sizeof(uint32_t));
      }
    }

    Serial.print("RX: ");
    Serial.print('\t');
    if (crc_rx_calc == crc_rx) {
      for (int i = 0; i < sizeof(RXdata) / sizeof(float); i++) {
        Serial.print(RXdata[i]);
        Serial.print('\t');
      }
    } else {
      Serial.print("Data Corrupted");
      Serial.print('\t');
    }
    Serial.println();
  }
}

int joystick_response(int pin, int input_low, int input_high, int output_low, int output_high, int blackpoint) {
  int input = analogRead(pin);
  int output = map(input, input_low, input_high, output_low, output_high);
  if (abs(output) <= blackpoint) {
    output = 0;
  }
  return output;
}
