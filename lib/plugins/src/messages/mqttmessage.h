#pragma once

#include "base/pluginmessages.h"

class MqttMessage : public PluginMessage
{
public:
    MqttMessage(int receiverId) : PluginMessage(0,receiverId) {}
    const char *topic;
    const uint8_t *payload;
    unsigned int length;
    bool appendTopic = true;
};

template <>
struct EntityIds<MqttMessage>
{
    enum { type_id = TYPEIDS::MQTTMESSAGE_TYPE };
};
