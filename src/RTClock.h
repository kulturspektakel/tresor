#include <RtcDS3231.h>
#include <Wire.h>

class RTClock {
  RtcDS3231<TwoWire> rtc = RtcDS3231<TwoWire>(Wire);
  void printDateTime(const RtcDateTime& dt);

  public:
    RTClock();
    void setup();
    long getTime();
};




