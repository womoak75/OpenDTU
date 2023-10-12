

#include "pluginmessagepublisher.h"
#include "plugin.h"

void PluginMessagePublisher::publish(const std::shared_ptr<PluginMessage> &m) {
  PDebug.printf(PDebugLevel::DEBUG, "system: %s publish %s to %s\n",
                PluginDebug::getPluginNameDebug(m.get()->getSenderId()),
                m.get()->getMessageTypeString(),
                PluginDebug::getPluginNameDebug(m.get()->getReceiverId()));
  process(m);
}

PluginMessagePublisher::PluginMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p)
    : plugins(p) {}

void PluginMessagePublisher::publishTo(
    int pluginId, const std::shared_ptr<PluginMessage> &mes) {
  if (isReceiver(pluginId, mes))
    getPluginById(pluginId)->internalCallback(mes);
}

void PluginMessagePublisher::publishToReceiver(
    const std::shared_ptr<PluginMessage> &mes) {
  int receiverId = mes.get()->getReceiverId();
  if (receiverId == 0) {
    for (unsigned int i = 0; i < getPluginCount(); i++) {
      publishTo(getPluginByIndex(i)->getId(), mes);
    }
  } else
    publishTo(receiverId, mes);
}

bool PluginMessagePublisher::isReceiver(
    int pluginId, const std::shared_ptr<PluginMessage> &mes) {
  return (isEnabled(pluginId) && mes.get()->getSenderId() != pluginId);
}

bool PluginMessagePublisher::isEnabled(int pluginId) {
  return (getPluginById(pluginId)->isEnabled());
}

Plugin *PluginMessagePublisher::getPluginByIndex(int index) {
  if (index >= 0 && index < plugins.size()) {
    return plugins[index].get();
  }
  return NULL;
}

Plugin *PluginMessagePublisher::getPluginById(int pluginid) {
  for (unsigned int i = 0; i < plugins.size(); i++) {
    if (plugins[i]->getId() == pluginid) {
      return plugins[i].get();
    }
  }
  return NULL;
}

PluginSingleQueueMessagePublisher::PluginSingleQueueMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p)
    : PluginMessagePublisher(p) {}

void PluginSingleQueueMessagePublisher::process(
    const std::shared_ptr<PluginMessage> &message) {
  queue.push(message);
}

void PluginSingleQueueMessagePublisher::loop() {
  while (queue.size() > 0l) {
    auto message = queue.pop().value();
    char buffer[128];
    message.get()->toString(buffer);
    unsigned long duration = millis();
    PDebug.printf(PDebugLevel::DEBUG,
                  "mainloop start @ core%d\n----\n%s\n----\n", xPortGetCoreID(),
                  buffer);

    PluginMessagePublisher::publishToReceiver(message);

    duration -= message.get()->getTS();
    PDebug.printf(PDebugLevel::DEBUG,
                  "----\n%s\nduration: %lu [ms]\n----\nmainloop end\n", buffer,
                  duration);

    // do i need this? :/
    message.reset();
  }
}

PluginMultiQueueMessagePublisher::PluginMultiQueueMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_)
    : PluginMessagePublisher(p), subscriptionForced(subscriptionForced_) {}

void PluginMultiQueueMessagePublisher::process(
    const std::shared_ptr<PluginMessage> &message) {
  for (unsigned int i = 0; i < getPluginCount(); i++) {
    int pluginId = getPluginByIndex(i)->getId();
    if (!(queues.find(pluginId) != queues.end())) {
      queues.insert({pluginId, std::make_shared<ThreadSafeMessageQueue>(
                                   ThreadSafeMessageQueue())});
    }
    if (isReceiver(pluginId, message))
      queues.at(pluginId)->push(message);
  }
}

bool PluginMultiQueueMessagePublisher::isReceiver(
    int pluginId, const std::shared_ptr<PluginMessage> &message) {

  bool r = PluginMessagePublisher::isReceiver(pluginId, message);
  if (r) {
    if (subscriptionForced && !(getPluginById(pluginId)->isSubscribed(message)))
      r = false;
  }
  return r;
}

void PluginMultiQueueMessagePublisher::loop() {
  bool hasMsg = false;
  for (auto &pair : queues) {
    auto queue = pair.second;
    if (queue.get()->size() > 0l)
      hasMsg = true;
    break;
  }
  if (hasMsg)
    PDebug.printf(PDebugLevel::WARN, "mainloop start @core%d\n----\n",
                  xPortGetCoreID());
  unsigned long mainduration = millis();
  for (auto &pair : queues) {
    auto queue = pair.second;
    while (queue.get()->size() > 0l) {
      auto message = queue.get()->pop().value();
      char buffer[256];
      message.get()->toString(buffer);
      unsigned long duration = millis();
      PluginMessagePublisher::publishTo(pair.first, message);
      duration = millis() - duration;
      PDebug.printf(PDebugLevel::WARN, "pluginqueue '%s' %lu [ms] - %s\n",
                    PluginDebug::getPluginNameDebug(pair.first), duration,
                    buffer);

      // queue->pop();
      //  do i need this? :/
      message.reset();
      yield();
    }
  }
  if (hasMsg) {
    mainduration = millis() - mainduration;
    PDebug.printf(PDebugLevel::WARN, "mainloop stop - %lu [ms]\n----\n",
                  mainduration);
  }
}

PluginMultiQueuePriorityMessagePublisher::
    PluginMultiQueuePriorityMessagePublisher(
        std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_)
    : PluginMessagePublisher(p), subscriptionForced(subscriptionForced_) {}

void PluginMultiQueuePriorityMessagePublisher::process(
    const std::shared_ptr<PluginMessage> &message) {
  for (unsigned int i = 0; i < getPluginCount(); i++) {
    int pluginId = getPluginByIndex(i)->getId();
    if (!(queues.find(pluginId) != queues.end())) {
      queues.insert({pluginId, std::make_shared<ThreadSafePriorityMessageQueue>(
                                   ThreadSafePriorityMessageQueue())});
    }
    if (isReceiver(pluginId, message))
      queues.at(pluginId)->push(message);
  }
}

bool PluginMultiQueuePriorityMessagePublisher::isReceiver(
    int pluginId, const std::shared_ptr<PluginMessage> &message) {

  bool r = PluginMessagePublisher::isReceiver(pluginId, message);
  if (r) {
    if (subscriptionForced && !(getPluginById(pluginId)->isSubscribed(message)))
      r = false;
  }
  return r;
}

void PluginMultiQueuePriorityMessagePublisher::loop() {
  bool hasMsg = false;
  for (auto &pair : queues) {
    auto queue = pair.second;
    if (queue.get()->size() > 0l)
      hasMsg = true;
    break;
  }
  if (hasMsg)
    PDebug.printf(PDebugLevel::WARN, "mainloop start @core%d\n----\n",
                  xPortGetCoreID());
  unsigned long mainduration = millis();
  for (auto &pair : queues) {
    auto queue = pair.second;
    while (queue.get()->size() > 0l) {
      auto message = queue.get()->pop().value();
      char buffer[256];
      message.get()->toString(buffer);
      unsigned long duration = millis();
      PluginMessagePublisher::publishTo(pair.first, message);
      duration = millis() - duration;
      PDebug.printf(PDebugLevel::WARN, "pluginqueue '%s' %lu [ms] - %s\n",
                    PluginDebug::getPluginNameDebug(pair.first), duration,
                    buffer);

      // queue->pop();
      //  do i need this? :/
      message.reset();
      yield();
    }
  }
  if (hasMsg) {
    mainduration = millis() - mainduration;
    PDebug.printf(PDebugLevel::WARN, "mainloop stop - %lu [ms]\n----\n",
                  mainduration);
  }
}
