#pragma once

#include "base/pluginmessages.h"

class MqttMessage : public PluginMessage {
public:
  MqttMessage(int senderId, int receiverId)
      : PluginMessage(TYPEIDS::MQTTMESSAGE_TYPE, senderId) {
    setReceiverId(receiverId);
  }
  ~MqttMessage() {

  }
  const char *topic;
  const uint8_t *payload;
  unsigned int length;
  bool appendTopic = true;
  void setMqtt(const char *topic, const uint8_t *payload, size_t len) {
    inlen = len;
    intopic = std::shared_ptr<char[]>(new char[strlen(topic)+1], std::default_delete<char[]>());
    strcpy(intopic.get(), topic);
    // hack
    indata = std::shared_ptr<uint8_t[]>(new uint8_t[len+1] , std::default_delete<uint8_t[]>());
    indata[len]='\0';
    memcpy(indata.get(), payload, len);
  }

  void toString(char *buffer) {
    sprintf(buffer, "MqttMessage{sender:%d, receiver:%d, type:%d}",
            getSenderId(), getReceiverId(), type_id);
  }

  std::shared_ptr<char[]> intopic;
  std::shared_ptr<uint8_t[]> indata;
  unsigned int inlen = 0;
};

template <> struct EntityIds<MqttMessage> {
  enum { type_id = TYPEIDS::MQTTMESSAGE_TYPE };
};
