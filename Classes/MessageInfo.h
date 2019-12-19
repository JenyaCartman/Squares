#pragma once

#include <cocos2d.h>
#include <enet/enet.h>
#include <ctime>
#include "PlayerInfo.h"

struct ClientRequest
{
    size_t ID;
    size_t index;
    float time;
    cocos2d::Vec2 direction; 
};


struct ServerResponse
{
    size_t index;
    PlayerInfo player;
};