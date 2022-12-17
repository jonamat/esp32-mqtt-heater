#include "Common.h"
#include "TxManage.h"
#include "SmartError.h"
#include <Arduino.h>
#include "../Utility/DemandManager.h"

void heaterOff() {
  TxManage.queueOffRequest();
  SmartError.inhibit();
}

void
CommStates::set(eCS eState) {
  _State = eState;
  _Count = 0;
  if (_report && _callback != NULL) {
    static const char* stateNames[] = {
      "Idle", "OEMCtrlRx", "OEMCtrlValidate", "HeaterRx1", "HeaterValidate1", "TxStart",
      "TxInterval", "HeaterRx2", "HeaterValidate2", "ExchangeComplete"
    };
    if (_State == Idle)
      _callback("\r\n");
    char msg[32];
    sprintf(msg, "State: %s\r\n", stateNames[_State]);
    _callback(msg);
  }
}

bool
CommStates::collectData(CProtocol& Frame, uint8_t val, int limit) {   // returns true when buffer filled
  Frame.Data[_Count++] = val;
  return _Count >= limit;
}

bool
CommStates::checkValidStart(uint8_t val) {
  if (_Count)
    return true;
  else
    return val == 0x76;
}

void
CommStates::setDelay(int ms) {
  _delay = millis() + ms;
}

bool
CommStates::delayExpired() {
  long test = millis() - _delay;
  return(test >= 0);
}
