#pragma once

#include "base/plugintypes.h"
#include "base/plugin.h"
#if __has_include("base/pluginids.h")
    #include "base/pluginids.h"
#endif

class Plugin;

enum class METADATA_TAGS { EMPTY_TAG, SENDERID, RECEIVERID, MSGTS, TESTTAG};
class MetaData : public ContainerMap<METADATA_TAGS,Entity> {

    public:
    int getSenderId() {return getValueAs<IntValue>(METADATA_TAGS::SENDERID).value;}
    int getReceiverId() {return getValueAs<IntValue>(METADATA_TAGS::RECEIVERID).value;}
    template <typename T>
    T getTagAs(METADATA_TAGS id) {
        return getValueAs<T>(id);
    }
    bool hasTag(METADATA_TAGS id) {
        return hasKey(id);
    }
    protected:
    void setSenderId(int id) { add(METADATA_TAGS::SENDERID,IntValue(0,id)); }
    void setReceiverId(int id) { add(METADATA_TAGS::RECEIVERID,IntValue(0,id)); }
    template <typename T>
    void addTag(METADATA_TAGS id, T tag) {
        add(id,tag);
    }

    friend class PluginMessage;
};

class EntityError {};

class PluginMessage :  public ContainerVector<Entity>, public Entity {

    public:
    PluginMessage(int senderid, int receiverid);
    PluginMessage(int senderid);
    PluginMessage(Plugin &plugin);

    PluginMessage(const PluginMessage &v) = default;
    //PluginMessage(int senderid) : PluginMessage(senderid,0) { }
    MetaData& getMetaData() { return headers; }
    bool hasData() { return (entities.size()>0);}
    bool from(int senderid) {
        return (headers.getSenderId()==senderid);
    }
    int getSenderId() { return headers.getSenderId(); }
    int getReceiverId() { return headers.getReceiverId(); }
    bool has(int senderid, int dataid) {
        return (from(senderid)&&hasDataId(dataid));
    }
    bool hasDataId(int id) {
        for(unsigned int i=0; i < getEntryCount() ; i++) {
            if(get(i).getId()==id)
                return true;
        }
        return false;
    }
    template <typename T>
    void addTag(METADATA_TAGS tag, T &e) {
        headers.addTag(tag,e);
    }
    template <typename T>
    void addTag(METADATA_TAGS tag, T &&e) {
        headers.addTag(tag,std::move(e));
    }
    template <typename T>
    T getDataAs(int dataid) {
        for(unsigned int i=0; i < getEntryCount() ; i++) {
            if(get(i).getId()==dataid)
                return getAs<T>(i).value;
        }
        throw(EntityError());
    }
    template <typename U>
    bool isMessageType()
    {
        return (EntityIds<U>::type_id == type_id);
    }
    template <typename U>
    U& getMessageAs()
    {
        auto v = std::static_pointer_cast<U>(this);
        return *v.get();
    }
    protected:
    MetaData headers;
 };

