// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <mutex>
#include <optional>
#include <queue>
#include "base/pluginmessages.h"

class ThreadSafeMessageQueue {
public:
  ThreadSafeMessageQueue() = default;
  ThreadSafeMessageQueue(const ThreadSafeMessageQueue &) = delete;
  ThreadSafeMessageQueue &operator=(const ThreadSafeMessageQueue &) = delete;

  ThreadSafeMessageQueue(ThreadSafeMessageQueue &&other) {
    std::lock_guard<std::mutex> lock(_mutex);
    container = std::move(other.container);
  }

  virtual ~ThreadSafeMessageQueue() {}

  unsigned long size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return container.size();
  }

  std::optional<std::shared_ptr<PluginMessage>> pop() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (container.empty()) {
      return std::nullopt;
    }
    std::shared_ptr<PluginMessage>tmp = container.front();
    container.pop();
    return tmp;
  }

  void push(const std::shared_ptr<PluginMessage> &item) {
    std::lock_guard<std::mutex> lock(_mutex);
    container.push(item);
  }

protected:

  bool empty() const { return container.empty(); }

  std::queue<std::shared_ptr<PluginMessage>> container;
  mutable std::mutex _mutex;
};

class PriorityMessageQueueCompare {
public:
  bool operator()(std::shared_ptr<PluginMessage> &m1,
                  std::shared_ptr<PluginMessage> m2) {
    return (m1.get()->getPriority() < m2.get()->getPriority());
  }
};

class ThreadSafePriorityMessageQueue {
public:
  ThreadSafePriorityMessageQueue() = default;
  ThreadSafePriorityMessageQueue(const ThreadSafePriorityMessageQueue &) = delete;
  ThreadSafePriorityMessageQueue &operator=(const ThreadSafePriorityMessageQueue &) = delete;

  ThreadSafePriorityMessageQueue(ThreadSafePriorityMessageQueue &&other) {
    std::lock_guard<std::mutex> lock(_mutex);
    container = std::move(other.container);
  }

  virtual ~ThreadSafePriorityMessageQueue() {}

  unsigned long size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return container.size();
  }

  std::optional<std::shared_ptr<PluginMessage>> pop() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (container.empty()) {
      return std::nullopt;
    }
    std::shared_ptr<PluginMessage> tmp = container.top();
    container.pop();
    return tmp;
  }

  void push(const std::shared_ptr<PluginMessage> &item) {
    std::lock_guard<std::mutex> lock(_mutex);
    container.push(item);
  }

protected:

  bool empty() const { return container.empty(); }

  std::priority_queue<std::shared_ptr<PluginMessage>,std::vector<std::shared_ptr<PluginMessage>>,PriorityMessageQueueCompare> container;
  mutable std::mutex _mutex;
};
