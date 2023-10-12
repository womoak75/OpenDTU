#pragma once

#include "base/ThreadSafePluginMessageQueue.h"
#include "base/pluginmessages.h"
#include <map>

// std::priority_queue<std::shared_ptr<PluginMessage>,
// std::vector<std::shared_ptr<PluginMessage>>,PriorityMessageQueueCompare>;

class PluginMessagePublisher {
public:
  PluginMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p);
  virtual ~PluginMessagePublisher() {}

  void publish(const std::shared_ptr<PluginMessage> &message);

  virtual void loop() {}
  void publishTo(int pluginId, const std::shared_ptr<PluginMessage> &mes);

  void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);
  virtual bool isReceiver(int pluginId,
                          const std::shared_ptr<PluginMessage> &mes);

protected:
  inline bool isEnabled(int pluginId);
  Plugin *getPluginById(int pluginid);
  Plugin *getPluginByIndex(int index);
  inline int getPluginCount() { return plugins.size(); }

  virtual void process(const std::shared_ptr<PluginMessage> &mes) {
    publishToReceiver(mes);
  }

private:
  std::vector<std::unique_ptr<Plugin>> &plugins;
};

class PluginSingleQueueMessagePublisher : public PluginMessagePublisher {
public:
  PluginSingleQueueMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p);
  virtual ~PluginSingleQueueMessagePublisher() {}

  void loop();

protected:
  virtual void process(const std::shared_ptr<PluginMessage> &message);

private:
  ThreadSafeMessageQueue queue;
};

class PluginMultiQueueMessagePublisher : public PluginMessagePublisher {
public:
  PluginMultiQueueMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p,
                                   bool subscriptionForced_ = true);
  virtual ~PluginMultiQueueMessagePublisher() {}

  void loop();
  virtual bool isReceiver(int pluginId,
                          const std::shared_ptr<PluginMessage> &mes);

protected:
  virtual void process(const std::shared_ptr<PluginMessage> &mes);

private:
  bool subscriptionForced;
  std::map<int, std::shared_ptr<ThreadSafeMessageQueue>> queues;
};

class PluginMultiQueuePriorityMessagePublisher : public PluginMessagePublisher {
public:
  PluginMultiQueuePriorityMessagePublisher(
      std::vector<std::unique_ptr<Plugin>> &p, bool subscriptionForced_ = true);
  virtual ~PluginMultiQueuePriorityMessagePublisher() {}

  void loop();
  virtual bool isReceiver(int pluginId,
                          const std::shared_ptr<PluginMessage> &mes);

protected:
  virtual void process(const std::shared_ptr<PluginMessage> &mes);

private:
  bool subscriptionForced;
  std::map<int, std::shared_ptr<ThreadSafePriorityMessageQueue>> queues;
};
