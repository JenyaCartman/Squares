#pragma once
#include <enet/enet.h>
#include <array>
#include <cocos2d.h>

struct PlayerInfo 
{
    size_t ID;
    cocos2d::Vec2 square;
    cocos2d::Vec2 direction;

    PlayerInfo(size_t id) : ID(id), square({50,50}), direction({ 0, 0 })
    {}
};