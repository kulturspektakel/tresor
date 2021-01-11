class Keypad {
  int address;
  int currentRow;
  int currentData;
  long beepUntil;
  long keylock;
  char entry[TOKEN_LENGTH + 1];

  char get_key();
  void pcf8574Write(int, int);
  int pcf8574Read(int);

 public:
  Keypad(int address);
  void setup();
  void loop();
  char* getEntry();
  void resetEntry();
};