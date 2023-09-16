#pragma once


class MqttMessage
{
public:
    const char *topic;
    const uint8_t *payload;
    unsigned int length;
    bool appendTopic = true;
};
