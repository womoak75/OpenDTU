#pragma once

#include "base/pluginmessages.h"

class PowerControlMessage : public PluginMessage {
public:
  PowerControlMessage(Plugin &p)
      : PluginMessage(TYPEIDS::POWERCONTROLMESSAGE_TYPE, p) {}
  ~PowerControlMessage() {}
  float power;
  String serialString;
  uint64_t serial;
  int toString(char *buffer) {
    int c = sprintf(buffer, "PowerControlMessage{base=");
    c = c + PluginMessage::toString(buffer+c);
    c = c + sprintf(buffer+c,"}");
    return c;
  }
};
template <> struct EntityIds<PowerControlMessage> {
  enum { type_id = TYPEIDS::POWERCONTROLMESSAGE_TYPE };
};