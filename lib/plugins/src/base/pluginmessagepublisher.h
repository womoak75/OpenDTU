#pragma once

#include "stdlib.h"
#include "messages/demomessage.h"
#include "messages/invertermessage.h"
#include "messages/metermessage.h"
#include "messages/mqttmessage.h"

class PluginMessagePublisher {
    public:
    PluginMessagePublisher(std::queue<std::shared_ptr<PluginMessage>>& q) : queue(q) {}
    virtual ~PluginMessagePublisher() {}
    template <typename T>
    void publish(T& message) {
        queue.push(std::make_shared<T>(message));
    }

    std::queue<std::shared_ptr<PluginMessage>>& queue;
};