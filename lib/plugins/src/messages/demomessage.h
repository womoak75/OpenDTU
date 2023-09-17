#pragma once

#include "base/pluginmessages.h"

class DemoMessage : public PluginMessage {
    public:
    DemoMessage() : PluginMessage(0,0) {
    }
    void setSomeValue(float f) {
        add(FloatValue(232324, f));
    }
    float getSomeValue() {
        return getAs<FloatValue>(232324).value;
    }
    
};
template <>
struct EntityIds<DemoMessage>
{
    enum { type_id = TYPEIDS::DEMOMESSAGE_TYPE };
};