#pragma once
#include "cocos2d.h"

#include <array>
#include <string>

struct PlayerInfo 
{
    unsigned int connectID;

    cocos2d::Vec2 points[4];

    PlayerInfo(unsigned int id) : connectID(id)
    {
        points[0] = cocos2d::Vec2(30, 30);
        points[1] = cocos2d::Vec2(30, 80);
        points[2] = cocos2d::Vec2(80, 80);
        points[3] = cocos2d::Vec2(80, 30);
    }

    std::string CreateMessage() const
    {
        std::string message = 
            std::to_string(static_cast<int>(points[0].x)) + "," + std::to_string(static_cast<int>(points[0].y)) + "," +
            std::to_string(static_cast<int>(points[1].x)) + "," + std::to_string(static_cast<int>(points[1].y)) + "," +
            std::to_string(static_cast<int>(points[2].x)) + "," + std::to_string(static_cast<int>(points[2].y)) + "," +
            std::to_string(static_cast<int>(points[3].x)) + "," + std::to_string(static_cast<int>(points[3].y)) + ",";

        return message;
    }
};