

#include "pluginmessages.h"

    PluginMessage::PluginMessage(int senderid, int receiverid) : Entity(TYPEIDS::PLUGINMESSAGE_TYPE) , headers() {
        headers.setSenderId(senderid);
        headers.setReceiverId(receiverid);
        headers.add(METADATA_TAGS::MSGTS,LongValue(0,millis()));
    }
    PluginMessage::PluginMessage(int senderid) : PluginMessage(senderid,0) {}
    PluginMessage::PluginMessage(Plugin &p) : PluginMessage((int)p.getId()) {}
