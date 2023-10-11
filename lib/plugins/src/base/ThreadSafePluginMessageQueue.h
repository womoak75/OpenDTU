// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <mutex>
#include <optional>
#include <queue>

template <typename Q, typename T> class ThreadSafeContainerQueue {
public:
  ThreadSafeContainerQueue() = default;
  ThreadSafeContainerQueue(const ThreadSafeContainerQueue<Q, T> &) = delete;
  ThreadSafeContainerQueue &
  operator=(const ThreadSafeContainerQueue<Q, T> &) = delete;

  ThreadSafeContainerQueue(ThreadSafeContainerQueue<Q, T> &&other) {
    std::lock_guard<std::mutex> lock(_mutex);
    container = std::move(other.container);
  }

  virtual ~ThreadSafeContainerQueue() {}

  unsigned long size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return container.size();
  }

  std::optional<T> pop() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (container.empty()) {
      return std::nullopt;
    }
    T tmp = getElement();
    container.pop();
    return tmp;
  }

  void push(const T &item) {
    std::lock_guard<std::mutex> lock(_mutex);
    container.push(item);
  }

protected:
  virtual T getElement();

  bool empty() const { return container.empty(); }

  Q container;
  mutable std::mutex _mutex;
};
