#pragma once
#include "cocos2d.h"
#include "PlayerInfo.h"
#include "Common/SharedConfig.h"

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

    CREATE_FUNC(MainScene);

private:
    void DrawWorld();

#ifndef SERVER
    Snapshot GetNewWorldState();
#endif //SERVER

private:
    Snapshot m_worldState;
    Snapshot m_newWorldState;
    cocos2d::Vec2 m_movement;

#ifdef SERVER
    ENetServer m_server;
#else 
    ENetClient m_client;

    int m_currentTick;
    int m_tickRate;
#endif //SERVER
};

