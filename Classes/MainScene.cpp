#include "MainScene.h"
#include "Common/SharedConfig.h"

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
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_A)
            m_movement.x = -1;

        if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_D)
            m_movement.x = 1;

        if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW || keyCode == EventKeyboard::KeyCode::KEY_W)
            m_movement.y = 1;

        if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW || keyCode == EventKeyboard::KeyCode::KEY_S)
            m_movement.y = -1;
    };

    eventListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event)
    {
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_A:
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_D:
            m_movement.x = 0;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
        case EventKeyboard::KeyCode::KEY_W:
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case EventKeyboard::KeyCode::KEY_S:
            m_movement.y = 0;
            break;
        }

    };

    m_currentTick = 0;
    m_tickRate = 3;

_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
#endif

    return true;
}

//#include <limits>
 
void MainScene::update(float delta)
{
    //DrawNode* p;
    //p->drawSolidRect()
    //std::numeric_limits<float>::epsilon();

    //float f1;
    //float f2;

    //if (std::abs(f1 - f2) <= std::numeric_limits<float>::epsilon()) {..}

#ifdef SERVER
    m_server.Listen();
    m_worldState = m_server.GetWorldState();
#else
    if (m_movement.x != 0 || m_movement.y != 0)
    {
        ClientRequest request;
        request.ID = m_client.GetID();
        request.index = m_client.GetRequestsSize();
        request.time = delta;
        request.direction = m_movement;

        m_client.AddRequest(request);

        m_currentTick++;
        if (m_currentTick == m_tickRate)
        {
            m_client.ProcessRequests();
            m_currentTick = 0;
        }
        
    }

    if (m_client.ReceiveMessage())
    {
        m_newWorldState = GetNewWorldState();
    }

    for (auto& newPlayerState : m_newWorldState)
    {
        auto currentPlayerState = std::find_if(m_worldState.begin(), m_worldState.end(), 
            [&](PlayerInfo player) { return newPlayerState.ID == player.ID; });

        if (abs(static_cast<int>(currentPlayerState->square.x) - static_cast<int>(newPlayerState.square.x)) <= 2
            && abs(static_cast<int>(currentPlayerState->square.y) - static_cast<int>(newPlayerState.square.y)) <= 2)
        {
            newPlayerState.direction = {0, 0};
        }

        currentPlayerState->square += newPlayerState.direction * params::speed * delta;
    }
#endif //SERVER

    DrawWorld();
}


#ifndef SERVER
Snapshot MainScene::GetNewWorldState()
{
    auto newWorldState = m_client.GetWorldState();

    for (auto& newPlayerState : newWorldState)
    {
        auto currentPlayerState = std::find_if(m_worldState.begin(), m_worldState.end(), 
            [&](PlayerInfo player) { return newPlayerState.ID == player.ID; });

        if (currentPlayerState == m_worldState.end())
        {
            m_worldState.push_back(newPlayerState);
        }
        else
        {
            newPlayerState.direction = newPlayerState.square - currentPlayerState->square;
            newPlayerState.direction.normalize();
        }
    }

    if (m_worldState.size() > newWorldState.size())
    {
        size_t index = 0;
        while (index < m_worldState.size())
        {
            auto newPlayerState = std::find_if(newWorldState.begin(), newWorldState.end(),
                [&](PlayerInfo player) { return m_worldState[index].ID == player.ID; });

            if (newPlayerState == newWorldState.end())
            {
                m_worldState.erase(m_worldState.begin() + index);
            }
            else
            {
                index++;
            }
        }
    }

    return newWorldState;
}
#endif //SERVER


void MainScene::DrawWorld()
{
    removeAllChildren();

    for (const auto& player : m_worldState)
    {
        auto* square = Sprite::create("cat.png");
        square->setScale(0.15);
        square->setPosition(player.square);
        addChild(square);
    }
}