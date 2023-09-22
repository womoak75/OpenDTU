

#include "pluginmessages.h"
#include "plugin.h"

    PluginMessage::PluginMessage(TYPEIDS tid, int senderId) : Entity(tid) {
        setSenderId(senderId);
        setReceiverId(0);
    }
    PluginMessage::PluginMessage(TYPEIDS tid, Plugin &p) : PluginMessage(tid,p.getId()) {}
    PluginMessage::PluginMessage(Plugin &p) : PluginMessage(TYPEIDS::PLUGINMESSAGE_TYPE,p.getId()) {}

void PluginMessage::toString(char* buffer) {
    sprintf(buffer,"PluginMessage{sender:%d, receiver:%d, type:%d}",senderId,receiverId,_id);
}
