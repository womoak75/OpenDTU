#pragma once

#include "base/PluginDebug.h"
#include "base/powercontrolalgo.hpp"
#include <array>

#define ARRAYSIZE 2
typedef struct {
  int input[ARRAYSIZE];
  int output[ARRAYSIZE];
} History;

class AvgPowercontrolAlgo : public PowercontrolAlgo {

public:
  AvgPowercontrolAlgo() : PowercontrolAlgo(), index(0), initialized(false) {
    init();
  }
  ~AvgPowercontrolAlgo() {}
  virtual bool calcLimit(std::shared_ptr<PowerControlStruct> powercontrol) {

    float newLimit = initialized ? getLimit(powercontrol->meters.getPower(),
                                            powercontrol->maxDiffW)
                                 : powercontrol->meters.getPower();
    saveHistory(powercontrol->meters.getPower(), newLimit);
    float threshold = std::abs(powercontrol->inverters.getLimit() - newLimit);
    if (threshold <= powercontrol->threshold) {
      PDebug.printf(PDebugLevel::DEBUG,
                    "powercontrol AvgPowercontrolAlgo: newlimit(%f) within "
                    "threshold(%f) -> no limit change\n",
                    newLimit, threshold);
    } else {
      PDebug.printf(PDebugLevel::DEBUG,
                    "powercontrol AvgPowercontrolAlgo: setting limit to %f\n",
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

private:
  void init() {
    for (int i = 0; i < ARRAYSIZE; i++) {
      history.input[i] = 0;
      history.output[i] = 0;
    }
  }
  void saveHistory(int in, int out) {
    history.input[index] = in;
    history.output[index] = out;
    index++;
    index = (index % ARRAYSIZE);
    if (!initialized && index == 0)
      initialized = true;
  }
  int getHistorySumIn() {
    int sum = 0;
    for (int i = 0; i < ARRAYSIZE; i++)
      sum += history.input[i];
    return sum;
  }
  int getHistorySumOut() {
    int sum = 0;
    for (int i = 0; i < ARRAYSIZE; i++)
      sum += history.output[i];
    return sum;
  }
  float getLimit(int in, int maxDiffW) {
    int avgIn = ((getHistorySumIn()) / (ARRAYSIZE));
    int diff = std::abs(avgIn - in);
    if (diff > maxDiffW)
      return ((avgIn + in) / 2);
    else
      return in;
  }
  History history;
  int index;
  bool initialized;
};
