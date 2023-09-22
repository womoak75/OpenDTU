#pragma once

#include "base/pluginmessages.h"

class DemoMessage : public PluginMessage {
    public:
    DemoMessage(Plugin& p) : PluginMessage(TYPEIDS::DEMOMESSAGE_TYPE,p) {
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