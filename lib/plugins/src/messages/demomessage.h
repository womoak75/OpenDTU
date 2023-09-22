#pragma once

#include "base/pluginmessages.h"

class DemoMessage : public PluginMessage {
public:
  DemoMessage(Plugin &p) : PluginMessage(TYPEIDS::DEMOMESSAGE_TYPE, p) {}
  void setSomeValue(float f) { somevalue = f; }
  float getSomeValue() { return somevalue; }
  void toString(char *buffer) {
    sprintf(buffer, "DemoMessage{sender:%d, receiver:%d, type:%d}",
            getSenderId(), getReceiverId(), type_id);
  }

private:
  float somevalue = 0;
};
template <> struct EntityIds<DemoMessage> {
  enum { type_id = TYPEIDS::DEMOMESSAGE_TYPE };
};