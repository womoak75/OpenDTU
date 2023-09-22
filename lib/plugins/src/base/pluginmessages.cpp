

#include "pluginmessages.h"
#include "plugin.h"

    PluginMessage::PluginMessage(TYPEIDS tid, int senderId) : Entity(tid) , headers() {
        headers.setSenderId(senderId);
        headers.setReceiverId(0);
        headers.add(METADATA_TAGS::MSGTS,LongValue(0,millis()));
    }
    PluginMessage::PluginMessage(TYPEIDS tid, Plugin &p) : PluginMessage(tid,p.getId()) {}
    PluginMessage::PluginMessage(Plugin &p) : PluginMessage(TYPEIDS::PLUGINMESSAGE_TYPE,p.getId()) {}

