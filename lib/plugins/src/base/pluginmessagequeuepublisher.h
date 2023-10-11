#pragma once

#include "pluginmessagepublisher.h"

template <class T>
class PluginQueueMessagePublisher : public PluginMessagePublisher {
public:
  PluginQueueMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p,
                              bool subscriptionForced_ = true);
  virtual ~PluginQueueMessagePublisher() {}

  void loop();

protected:
  virtual void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);
  virtual void publishToAll(const std::shared_ptr<PluginMessage> &message);
  virtual void publishTo(int pluginId,
                         const std::shared_ptr<PluginMessage> &message);
  bool subscriptionForced;

  std::map<int, std::shared_ptr<T>> queues;
};
typedef PluginQueueMessagePublisher<PluginMessageQueue>
    PluginMultiQueueMessagePublisher;
typedef PluginQueueMessagePublisher<PluginPriorityMessageQueue>
    PluginMultiQueuePriorityMessagePublisher;

/*
class PluginMultiQueuePriorityMessagePublisher
    : public PluginMultiQueueMessagePublisher {

public:
  PluginMultiQueuePriorityMessagePublisher(
      std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_ = true);
  virtual ~PluginMultiQueuePriorityMessagePublisher() {}

protected:
  std::map<int, std::shared_ptr<PluginPriorityMessageQueue>> queues;
};
*/