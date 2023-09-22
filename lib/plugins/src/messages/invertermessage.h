#pragma once

#include "base/pluginmessages.h"

class InverterMessage : public PluginMessage
{
public:
    InverterMessage(Plugin& p) : PluginMessage(TYPEIDS::INVERTERMESSAGE_TYPE,p) {}
    ~InverterMessage() {}
    uint64_t inverterSerial;
    String inverterStringSerial;
    int channelNumber;
    int channelType;
    /**
     *  fieldId - see hmDefines.h => field types
     */
    int fieldId;
    /**
    *value - value transmited by inverter
    */
    float value;
};

template <>
struct EntityIds<InverterMessage>
{
    enum { type_id = TYPEIDS::INVERTERMESSAGE_TYPE };
};