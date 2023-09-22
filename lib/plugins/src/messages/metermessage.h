#pragma once

#include "base/pluginmessages.h"

class MeterMessage : public PluginMessage {
    public:
    MeterMessage(Plugin& p) : PluginMessage(TYPEIDS::METERMESSAGE_TYPE,p) {
    }
    ~MeterMessage() {}
    void setPowerValue(float power) {
        add(FloatValue(METER_POWER, power));
    }
    void setMeterSerial(String serial) {
        add(StringValue(METER_SERIAL, serial));
    }
};
template <>
struct EntityIds<MeterMessage>
{
    enum { type_id = TYPEIDS::METERMESSAGE_TYPE };
};