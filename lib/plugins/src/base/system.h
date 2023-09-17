#pragma once

#include "globals.h"
#include "plugin.h"
#include "pluginmessages.h"

class Plugin;
class System
{
public:
    virtual void subscribeMqtt(Plugin *plugin, char *topic, bool append) = 0;
    virtual bool enqueueMessage(Plugin *sender, char *topic, char *data, bool append) = 0;
    virtual void publishMessage(Plugin *sender, PluginMessage& message) = 0;
    virtual void addTimerCb(Plugin *plugin, const char* timername, PLUGIN_TIMER_INTVAL intval, uint32_t interval, std::function<void(void)> timerCb) = 0;
    virtual Plugin *getPluginById(int pluginid) = 0;
    virtual Plugin *getPluginByName(const char *pluginname) = 0;
    virtual int getPluginCount() = 0;
};

