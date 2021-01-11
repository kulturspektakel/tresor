#include "RTClock.h"
#include "Keypad.h"
#include "sha1.h"
#include "TOTP.h"

Keypad keypad = Keypad(0x20);
RTClock rtclock = RTClock();
uint8_t hmacKey[] = {0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72};
TOTP totp = TOTP(hmacKey, 10);

char validTokens[NUMBER_OF_TOKENS * TOKEN_LENGTH] = "";
int currentIndex = 0;
unsigned long lastTimeUpdated;
unsigned long doorOpened;


void setup() {
  Serial.begin(115200);
  keypad.setup();
  rtclock.setup();
  pinMode(A2, OUTPUT);
}

void openLock() {
  Serial.println("open");
  digitalWrite(A2, HIGH);
  doorOpened = millis();
  keypad.resetEntry();
}

void loop() {
  keypad.loop();

  if (lastTimeUpdated < millis() - 1000) {
    lastTimeUpdated = millis();
    char* newCode = totp.getCode(rtclock.getTime());
    int prevIndex = (currentIndex - 1) % 5;
    if (strncmp(newCode, &validTokens[prevIndex * TOKEN_LENGTH], TOKEN_LENGTH) != 0) {
      strncpy(&validTokens[currentIndex * TOKEN_LENGTH], newCode, TOKEN_LENGTH);
      currentIndex = (currentIndex + 1) % NUMBER_OF_TOKENS;
      Serial.print("Valid tokens: ");
      Serial.println(validTokens);
    }
  }

  for (int i = 0; i < NUMBER_OF_TOKENS; i++) {
    if (strlen(keypad.getEntry()) == 6 && strncmp(keypad.getEntry(), &validTokens[i * TOKEN_LENGTH], TOKEN_LENGTH) == 0) {
      openLock();
    }
  }

  if (strlen(keypad.getEntry()) == 6 && strncmp(keypad.getEntry(), "*ACDC#", TOKEN_LENGTH) == 0) {
    openLock();
  }

  if (doorOpened > 0 && doorOpened < millis() - 5000) {
    Serial.println("close");
    digitalWrite(A2, LOW);
    doorOpened = 0;
  }
}

