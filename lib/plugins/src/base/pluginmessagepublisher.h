#pragma once

#include "base/ThreadSafePluginMessageQueue.h"
#include "base/pluginmessages.h"
#include <map>

class PluginMessageQueue : public ThreadSafeContainerQueue<
                               std::queue<std::shared_ptr<PluginMessage>>,
                               std::shared_ptr<PluginMessage>> {
protected:
  std::shared_ptr<PluginMessage> getElement() { return container.front(); }
};

class PriorityMessageQueueCompare {
public:
  bool operator()(std::shared_ptr<PluginMessage> &m1,
                  std::shared_ptr<PluginMessage> m2) {
    return (m1.get()->getPriority() < m2.get()->getPriority());
  }
};

class PluginPriorityMessageQueue
    : public ThreadSafeContainerQueue<
          std::priority_queue<std::shared_ptr<PluginMessage>,
                              std::vector<std::shared_ptr<PluginMessage>>,
                              PriorityMessageQueueCompare>,
          std::shared_ptr<PluginMessage>> {
protected:
  std::shared_ptr<PluginMessage> getElement() { return container.top(); }
};

class PluginMessagePublisher {
public:
  PluginMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p);
  virtual ~PluginMessagePublisher() {}

  void publish(const std::shared_ptr<PluginMessage> &message);

  virtual void loop() {}

protected:
  Plugin *getPluginById(int pluginid);
  Plugin *getPluginByIndex(int index);

  int getPluginCount() { return plugins.size(); }

  virtual void publishTo(int pluginId,
                         const std::shared_ptr<PluginMessage> &mes);

  virtual void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);

  virtual void publishToAll(const std::shared_ptr<PluginMessage> &message);

private:
  std::vector<std::unique_ptr<Plugin>> &plugins;
};

class PluginSingleQueueMessagePublisher : public PluginMessagePublisher {
public:
  PluginSingleQueueMessagePublisher(std::vector<std::unique_ptr<Plugin>> &p);
  virtual ~PluginSingleQueueMessagePublisher() {}

  void loop();

protected:
  virtual void publishTo(int pluginId,
                         const std::shared_ptr<PluginMessage> &mes);
  virtual void publishToReceiver(const std::shared_ptr<PluginMessage> &mes);

  virtual void publishToAll(const std::shared_ptr<PluginMessage> &message);

private:
  PluginMessageQueue queue;
};
