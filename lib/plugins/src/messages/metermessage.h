#pragma once

#include "base/pluginmessages.h"

class MeterMessage : public PluginMessage {
public:
  MeterMessage(Plugin &p) : PluginMessage(TYPEIDS::METERMESSAGE_TYPE, p) {}
  ~MeterMessage() {}
  void setPowerValue(float power_) { power = power_; }
  float power;
  String serial;
  int toString(char *buffer) {
    int c = sprintf(buffer, "MeterMessage{base=");
    c = c + PluginMessage::toString(buffer+c);
    c = c + sprintf(buffer+c,"}");
    return c;
  }
};
template <> struct EntityIds<MeterMessage> {
  enum { type_id = TYPEIDS::METERMESSAGE_TYPE };
};