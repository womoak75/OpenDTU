#pragma once

#include "base/pluginmessages.h"

class InverterMessage : public PluginMessage
{
public:
    InverterMessage(Plugin& p) : PluginMessage(p) { type_id = TYPEIDS::INVERTERMESSAGE_TYPE;}
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