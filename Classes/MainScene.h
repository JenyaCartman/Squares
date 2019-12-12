#pragma once
#include "cocos2d.h"
#include "PlayerInfo.h"

#include <array>
#include <vector>

#ifdef SERVER
#include "Server/ENetServer.h"
#else
#include "Client/ENetClient.h"
#endif //SERVER

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;
    void update(float delta) override;

    void AddPlayer(const PlayerInfo& player);

    CREATE_FUNC(MainScene);

private:
    void ParseMessage(const std::string& message);
    void DrawWorld();

private:
    std::vector<PlayerInfo> m_worldState;

#ifdef SERVER
    ENetServer m_server;
#else 
    ENetClient m_client;
#endif //SERVER
};

