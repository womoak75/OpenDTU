#pragma once

#include "pluginmessagepublisher.h"

class PluginMultiQueueMessagePublisher : public PluginMessagePublisher {
public:
  PluginMultiQueueMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p,
                              bool subscriptionForced_ = true);
  virtual ~PluginMultiQueueMessagePublisher() {}

  void loop();

protected:
  virtual void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);
  virtual void publishToAll(const std::shared_ptr<PluginMessage> &message);
  virtual void publishTo(int pluginId,
                         const std::shared_ptr<PluginMessage> &message);
  
  private:
  bool subscriptionForced;
  std::map<int, std::shared_ptr<ThreadSafeMessageQueue>> queues;
};


class PluginMultiQueuePriorityMessagePublisher : public PluginMessagePublisher {
public:
  PluginMultiQueuePriorityMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p,
                              bool subscriptionForced_ = true);
  virtual ~PluginMultiQueuePriorityMessagePublisher() {}

  void loop();

protected:
  virtual void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);
  virtual void publishToAll(const std::shared_ptr<PluginMessage> &message);
  virtual void publishTo(int pluginId,
                         const std::shared_ptr<PluginMessage> &message);
  
  private:
  bool subscriptionForced;
  std::map<int, std::shared_ptr<ThreadSafePriorityMessageQueue>> queues;
};

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