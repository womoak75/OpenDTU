// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Hoymiles.h>
#include <TaskSchedulerDeclarations.h>

class WebApiWsLiveClass {
public:
    WebApiWsLiveClass();
    void init(AsyncWebServer& server, Scheduler& scheduler);

private:
    void generateJsonResponse(JsonVariant& root);
    void addField(JsonObject& root, std::shared_ptr<InverterAbstract> inv, const ChannelType_t type, const ChannelNum_t channel, const FieldId_t fieldId, String topic = "");
    void addTotalField(JsonObject& root, const String& name, const float value, const String& unit, const uint8_t digits);
    void onLivedataStatus(AsyncWebServerRequest* request);
    void onWebsocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);

    AsyncWebServer* _server;
    AsyncWebSocket _ws;

    uint32_t _lastWsPublish = 0;
    uint32_t _newestInverterTimestamp = 0;

    std::mutex _mutex;

    Task _wsCleanupTask;
    void wsCleanupTaskCb();

    Task _sendDataTask;
    void sendDataTaskCb();
};
