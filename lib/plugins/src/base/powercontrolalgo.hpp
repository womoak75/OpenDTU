#pragma once

#include "base/pluginstructs.h"

class PowercontrolAlgo {
public:
  PowercontrolAlgo() {}
  virtual ~PowercontrolAlgo() {}
  virtual bool calcLimit(std::shared_ptr<PowerControlStruct> powercontrol) { return false; };
};
