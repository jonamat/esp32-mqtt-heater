#pragma once

#include "Arduino.h"

#define LOWERLIMIT(A, B) { if((A) < (B)) (A) = (B); }
#define UPPERLIMIT(A, B) { if((A) > (B)) (A) = (B); }
#define BOUNDSLIMIT(A, B, C) { if((A) < (B)) (A) = (B); if((A) > (C)) (A) = (C); }
#define INBOUNDS(TST, MIN, MAX) (((TST) >= (MIN)) && ((TST) <= (MAX)))

class CProtocol;

// a class to generate time stamps depending if a heater or otherwise frame header is presented
class CContextTimeStamp {
  unsigned long prevTime;
  unsigned long refTime;
public:
  CContextTimeStamp() {
    refTime = 0;
    prevTime = 0;
  };
  void setRefTime() {
    refTime = millis();
  };
  void report(bool isDelta, char* msg = NULL) {
    if (isDelta) {
      long delta = millis() - prevTime;
      if (msg)
        sprintf(msg, "%+8ldms ", delta);
      else
        Serial.printf("%+8ldms ", delta);
    }
    else {
      prevTime = millis();
      if (msg)
        sprintf(msg, "%8ldms ", prevTime - refTime);
      else
        Serial.printf("%8ldms ", prevTime - refTime);
    }
  };
  void report(char* msg = NULL) {
    prevTime = millis();
    if (msg) {
      sprintf(msg, "%8ldms ", prevTime - refTime);
    }
    else
      Serial.printf("%8ldms ", prevTime - refTime);
  };
};

class CommStates {
public:
  // comms states
  enum eCS {
    Idle, OEMCtrlRx, OEMCtrlValidate, HeaterRx1, HeaterValidate1, TxStart, TxInterval, HeaterRx2, HeaterValidate2, ExchangeComplete
  };

private:
  eCS _State;
  int _Count;
  unsigned long _delay;
  bool _report;
  std::function<void(const char*)> _callback;
public:
  CommStates() {
    _State = Idle;
    _Count = 0;
    _delay = millis();
    _report = false;
    _callback = NULL;
  }
  void set(eCS eState);
  eCS get() {
    return _State;
  }
  bool is(eCS eState) {
    return _State == eState;
  }
  bool collectData(CProtocol& Frame, uint8_t val, int limit = 24);
  bool checkValidStart(uint8_t val);
  void setDelay(int ms);
  bool delayExpired();
  bool toggleReporting() {
    _report = !_report;
    return isReporting();
  };
  bool isReporting() {
    return _report != 0;
  };
  void setCallback(std::function<void(const char*)> fn) { _callback = fn; };
};


// a class to collect a new data byte from the blue wire
class sRxData {
  bool newData;
  int  Value;
public:
  sRxData() {
    reset();
  }
  void reset() {
    newData = false;
    Value = 0;
  }
  void setValue(int value) {
    newData = true;
    Value = value;
  }
  bool available() {
    return newData;
  }
  int getValue() {
    return Value;
  }
};

// a class to collect rx bytes into a string, typ. until a line terminator (handled elsewhere)
struct sRxLine {
  char Line[1024];
  int  Len;
  sRxLine() {
    clear();
  }
  bool append(char val) {
    if (Len < (sizeof(Line) - 1)) {
      Line[Len++] = val;
      Line[Len] = 0;
      return true;
    }
    return false;
  }
  void clear() {
    Line[0] = 0;
    Len = 0;
  }
};

extern const char* getBlueWireStatStr();
extern int   getBlueWireStat();
extern void DebugReportFrame(const char* hdr, const CProtocol& Frame, const char* ftr, char* msg);
extern int   getSmartError();
extern void heaterOff();
