#pragma once

#include "base/PluginDebug.h"
#include "base/powercontrolalgo.hpp"

class DefaultPowercontrolAlgo : public PowercontrolAlgo {
public:
  DefaultPowercontrolAlgo() : PowercontrolAlgo() {}
  ~DefaultPowercontrolAlgo() {}
  virtual bool calcLimit(std::shared_ptr<PowerControlStruct> powercontrol) {
    PDebug.printf(PDebugLevel::DEBUG,
                  "powercontrol PowercontrolAlgo: consumption=%f "
                  "production=%f limit=%f\n",
                  powercontrol->meters.getPower(),
                  powercontrol->inverters.getPower(),
                  powercontrol->inverters.getLimit());
    // TODO: do some magic calculation here
    // :/
    // float newlimit =
    // magicFunction(powercontrol.production,powercontrol.consumption);

    float newLimit = powercontrol->meters.getPower();
    float threshold = std::abs(powercontrol->inverters.getLimit() - newLimit);
    if (threshold <= powercontrol->threshold) {
      PDebug.printf(PDebugLevel::DEBUG,
                    "powercontrol PowercontrolAlgo: newlimit(%f) within "
                    "threshold(%f) -> no limit change\n",
                    newLimit, threshold);
    } else {
      PDebug.printf(PDebugLevel::DEBUG,
                    "powercontrol PowercontrolAlgo: setting limit to %f\n",
                    newLimit);
      setLimit(powercontrol, newLimit);
      return true;
    }
    return false;
  }

  void setLimit(std::shared_ptr<PowerControlStruct> &powercontrol,
                float newLimit) {
    // we split the limit among inverters
    // not pretty smart if more than 1 is configured
    // todo: split by capacity of inverter?
    float limit = (newLimit / powercontrol->inverters.count());
    PDebug.printf(PDebugLevel::DEBUG,
                  "powercontrol PowercontrolAlgo: split limit(%f) between "
                  "inverters(%d) %f each\n",
                  newLimit, powercontrol->inverters.count(), limit);
    for (int i = 0; i < powercontrol->inverters.count(); i++)
      powercontrol->inverters[i]->setNewLimit(limit);
  }
};
