#pragma once

#include "base/pluginmessages.h"

class MeterMessage : public PluginMessage {
public:
  MeterMessage(Plugin &p) : PluginMessage(TYPEIDS::METERMESSAGE_TYPE, p) {}
  ~MeterMessage() {}
  void setPowerValue(float power_) { power = power_; }
  float power;
  String serial;
  void toString(char *buffer) {
    sprintf(buffer, "MeterMessage{sender:%d, receiver:%d, type:%d}",
            getSenderId(), getReceiverId(), type_id);
  }
};
template <> struct EntityIds<MeterMessage> {
  enum { type_id = TYPEIDS::METERMESSAGE_TYPE };
};