#include "Keypad.h"
#include <Arduino.h>
#include <Wire.h>

#define ROW0 0
#define ROW1 1
#define ROW2 2
#define ROW3 3
#define COL0 4
#define COL1 5
#define COL2 6
#define COL3 7

const char keymap[4][5] = {"123A", "456B", "789C", "*0#D"};

// Hex byte statement for each port of PCF8574
const int hexData[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// Hex data for each row of keypad in PCF8574
const int pcf8574RowData[4] = {
    hexData[ROW1] | hexData[ROW2] | hexData[ROW3] | hexData[COL0] |
        hexData[COL1] | hexData[COL2] | hexData[COL3],
    hexData[ROW0] | hexData[ROW2] | hexData[ROW3] | hexData[COL0] |
        hexData[COL1] | hexData[COL2] | hexData[COL3],
    hexData[ROW0] | hexData[ROW1] | hexData[ROW3] | hexData[COL0] |
        hexData[COL1] | hexData[COL2] | hexData[COL3],
    hexData[ROW0] | hexData[ROW1] | hexData[ROW2] | hexData[COL0] |
        hexData[COL1] | hexData[COL2] | hexData[COL3],
};

// Hex data for each col of keypad in PCF8574
const int col[4] = {hexData[COL0], hexData[COL1], hexData[COL2], hexData[COL3]};


Keypad::Keypad(int a) {
  address = a;
}


void Keypad::pcf8574Write(int addr, int data) {
  currentData = data;
  Wire.beginTransmission(addr);
  Wire.write(data);
  Wire.endTransmission();
}

int Keypad::pcf8574Read(int addr) {
  Wire.requestFrom(addr, 1);
  return Wire.read();
}

char Keypad::get_key() {
  static int lastKey;
  static bool keydown;
  int readData;
  int key = '\0';

  // Search row low
  pcf8574Write(address, pcf8574RowData[currentRow]);

  for (int r = 0; r < 4; r++) {
    // Read pcf8574 port data
    readData = pcf8574Read(address);

    // XOR to compare obtained data and current data and know if some column are low
    readData ^= currentData;
    // Key pressed!
    if (col[r] == readData) {
      lastKey = keymap[currentRow][r];
      if (!keydown) {
        keydown = true;
        return lastKey;
      }
      return '\0';
    }
  }

  // Key was pressed and then released
  if (key == '\0' && lastKey != '\0') {
    lastKey = '\0';
    keydown = false;
    return lastKey;
  }

  // All PCF8574 ports high again
  pcf8574Write(address, 0xff);

  // Next row
  currentRow = (currentRow + 1) % 4;

  return key;
}


void Keypad::setup() {
  pinMode(A3, OUTPUT);
  // keyboard
  pcf8574Write(address, 0xff);
  currentRow = 0;
}

void Keypad::loop() {
  char key = get_key();
  if (key != '\0' && keylock < millis()) {
    keylock = millis() + 300;
    beepUntil = millis() + 100;
    digitalWrite(A3, HIGH);
    int len = strlen(entry);
    if (len < TOKEN_LENGTH) {
      entry[len] = key;
      entry[len + 1] = '\0';
    } else {
      for (int i = 0; i < TOKEN_LENGTH - 1; i++) {
        entry[i] = entry[i+1];
      }
      entry[TOKEN_LENGTH - 1] = key;
      entry[TOKEN_LENGTH] = '\0';
    }

    Serial.println(entry);
  }

  if (beepUntil > 0  && beepUntil < millis()) {
    beepUntil = 0;
    digitalWrite(A3, LOW);
  }
}

char* Keypad::getEntry() {
  return entry;
}

void Keypad::resetEntry() {
  entry[0] = '\0';
}





