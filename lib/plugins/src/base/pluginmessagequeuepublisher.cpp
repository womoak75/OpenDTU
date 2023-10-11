

#include "pluginmessagequeuepublisher.h"
#include "plugin.h"

template <class T>
PluginQueueMessagePublisher<T>::PluginQueueMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_)
    : PluginMessagePublisher(p), subscriptionForced(subscriptionForced_) {}

template <class T>
void PluginQueueMessagePublisher<T>::publishTo(
    int pluginId, const std::shared_ptr<PluginMessage> &message) {
  if (message.get()->getSenderId() == pluginId)
    return;

  if (!(queues.find(pluginId) != queues.end())) {
    queues.insert(
        {pluginId, std::make_shared<T>(T())});
  }
  queues.at(pluginId)->push(message);
}
template <class T>
void PluginQueueMessagePublisher<T>::publishToReceiver(
    const std::shared_ptr<PluginMessage> &message) {
  publishTo(message->getReceiverId(), message);
}
template <class T>
void PluginQueueMessagePublisher<T>::publishToAll(
    const std::shared_ptr<PluginMessage> &message) {
  for (int i = 0; i < getPluginCount(); i++) {
    auto plugin = getPluginByIndex(i);
    /*
    PDebug.printf(PDebugLevel::WARN, "%s is subscribed for %s(%d) ? %d\n",
                  PluginDebug::getPluginNameDebug(plugin->getId()),
                  message.get()->getMessageTypeString(),
                  message.get()->getMessageTypeId(),
                  plugin->isSubscribed(message));
                  */
    if (subscriptionForced && !plugin->isSubscribed(message))
      continue;
    publishTo(plugin->getId(), message);
  }
}

template <class T>
void PluginQueueMessagePublisher<T>::loop() {
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

      //queue->pop();
      // do i need this? :/
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
/*
PluginMultiQueuePriorityMessagePublisher::
    PluginMultiQueuePriorityMessagePublisher(
        std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_)
    : PluginMultiQueueMessagePublisher(p, subscriptionForced_) {}
*/