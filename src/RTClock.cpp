#include "RTClock.h"

RTClock::RTClock() {}

void RTClock::setup() {
  rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  
  if (!rtc.IsDateTimeValid()) {
    if (rtc.LastError() != 0) {
      Serial.print("RTC communications error = ");
      Serial.println(rtc.LastError());
    } else {
      Serial.println("RTC lost confidence in the DateTime!");
      rtc.SetDateTime(compiled);
    }
  }
  if (!rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }

  RtcDateTime now = rtc.GetDateTime();
  printDateTime(now);
  
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println(
        "RTC is the same as compile time! (not expected but all is fine)");
  }
  rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

long RTClock::getTime() {
  return rtc.GetDateTime().Epoch64Time();
}


#define countof(a) (sizeof(a) / sizeof(a[0]))

void RTClock::printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring, countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(),
             dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  Serial.println(datestring);
}






