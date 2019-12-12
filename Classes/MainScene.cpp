#include "MainScene.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

bool MainScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    scheduleUpdate();

#ifndef SERVER
    while (!m_client.Connect());

auto eventListener = EventListenerKeyboard::create();

eventListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case EventKeyboard::KeyCode::KEY_A:
        m_client.Send("left");
        break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case EventKeyboard::KeyCode::KEY_D:
        m_client.Send("right");
        break;
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
    case EventKeyboard::KeyCode::KEY_W:
        m_client.Send("up");
        break;
    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case EventKeyboard::KeyCode::KEY_S:
        m_client.Send("down");
        break;
    }
};

_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
#endif

    return true;
}

void MainScene::update(float delta)
{
#ifdef SERVER
    m_server.Listen();
    m_worldState = m_server.GetWorldState();
#else
    if (m_client.CheckMessages())
    {
        auto message = m_client.GetLastMessage();
        ParseMessage(message);
    }
#endif //SERVER

    DrawWorld();
}


void MainScene::ParseMessage(const std::string& message)
{
    size_t end = 0;
    m_worldState.clear();

    while (end != message.size())
    {
        std::array<int, 8> result;
        size_t begin = end;
        end = message.find_first_of('.', begin) + 1;
        auto playerMessage = message.substr(begin, end - begin - 1);

        int pos = 0;
        std::string tmp = "";

        for (int i = 0; i < message.size() && pos < 8; ++i)
        {
            if (message[i] != ',')
            {
                tmp += message[i];
            }
            else
            {
                result[pos] = std::stoi(tmp);
                pos++;
                tmp = "";
            }
        }

        PlayerInfo tmpPlayer(0);
        tmpPlayer.points[0] = Vec2(result[0], result[1]);
        tmpPlayer.points[1] = Vec2(result[2], result[3]);
        tmpPlayer.points[2] = Vec2(result[4], result[5]);
        tmpPlayer.points[3] = Vec2(result[6], result[7]);

        m_worldState.push_back(tmpPlayer);
    }
}


void MainScene::DrawWorld()
{
    removeAllChildren();
    for (const auto& player : m_worldState)
    {
        auto square = DrawNode::create();
        square->drawPolygon(player.points, 4, Color4F::WHITE, 1, Color4F::WHITE);
        addChild(square);
    }
}