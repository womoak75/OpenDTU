#pragma once

#include "parser/StatisticsParser.h"

class InverterMessage
{
public:
     /**
     * invererSerial - id of inverter 
     */
    uint64_t inverterSerial;
    String inverterStringSerial;
    
    ChannelNum_t channelNumber;
    ChannelType_t channelType;
    /**
     *  fieldId - see hmDefines.h => field types
     */
    FieldId_t fieldId;
    /**
    *value - value transmited by inverter
    */
    float value;
};
