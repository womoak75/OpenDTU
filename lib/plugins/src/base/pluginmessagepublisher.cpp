

#include "pluginmessagepublisher.h"
#include "plugin.h"

PluginSingleQueueMessagePublisher::PluginSingleQueueMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p)
    : PluginMessagePublisher(p) {}

void PluginMessagePublisher::publish(const std::shared_ptr<PluginMessage> &m) {
  PDebug.printf(PDebugLevel::DEBUG, "system: %s publish %s to %s\n",
                PluginDebug::getPluginNameDebug(m.get()->getSenderId()),
                m.get()->getMessageTypeString(),
                PluginDebug::getPluginNameDebug(m.get()->getReceiverId()));
  if (m->isBroadcast())
    publishToAll(m);
  else
    publishToReceiver(m);
}

PluginMessagePublisher::PluginMessagePublisher(
    std::vector<std::unique_ptr<Plugin>> &p)
    : plugins(p) {}

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

void PluginMessagePublisher::publishTo(
    int pluginId, const std::shared_ptr<PluginMessage> &mes) {
  if (mes.get()->getSenderId() == pluginId)
    return;
  Plugin *p = getPluginById(pluginId);
  if (NULL != p && p->isEnabled()) {
    p->internalCallback(mes);
  }
}

void PluginMessagePublisher::publishToReceiver(
    const std::shared_ptr<PluginMessage> &mes) {
  publishTo(mes->getReceiverId(), mes);
}

void PluginMessagePublisher::publishToAll(
    const std::shared_ptr<PluginMessage> &message) {
  int pcount = getPluginCount();
  PDebug.printf(PDebugLevel::DEBUG,
                "PluginMessagePublisher::publishToAll count:%d\n", pcount);
  for (unsigned int i = 0; i < pcount; i++) {
    Plugin *p = getPluginByIndex(i);
    publishTo(p->getId(), message);
  }
};

void PluginSingleQueueMessagePublisher::publishToReceiver(
    const std::shared_ptr<PluginMessage> &message) {
  queue.push(message);
}

void PluginSingleQueueMessagePublisher::publishToAll(
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
    if (message->getReceiverId() != 0) {
      PluginMessagePublisher::publishToReceiver(message);
    } else {
      PluginMessagePublisher::publishToAll(message);
    }
    duration -= message.get()->getTS();
    PDebug.printf(PDebugLevel::DEBUG,
                  "----\n%s\nduration: %lu [ms]\n----\nmainloop end\n", buffer,
                  duration);

    // do i need this? :/
    message.reset();
  }
}
