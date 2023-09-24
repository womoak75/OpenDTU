#pragma once

typedef struct {
  String inverterId;
  String meterSerial;
  float limit = 0.0;
  uint32_t threshold = 0;
  float consumption = 0.0;
  float production = 0.0;
  bool update = false;
} powercontrolstruct;
class PowercontrolAlgo {
public:
  PowercontrolAlgo() {}
  virtual bool calcLimit(powercontrolstruct &powercontrol) { return false; };
};
