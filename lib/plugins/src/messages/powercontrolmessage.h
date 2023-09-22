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
  void toString(char *buffer) {
    sprintf(buffer, "PowerControlMessage{sender:%d, receiver:%d, type:%d}",
            getSenderId(), getReceiverId(), type_id);
  }
};
template <> struct EntityIds<PowerControlMessage> {
  enum { type_id = TYPEIDS::POWERCONTROLMESSAGE_TYPE };
};