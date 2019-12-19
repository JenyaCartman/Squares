#pragma once

#include <cstdlib>
#include <enet/enet.h>
#include <cocos2d.h>
#include <array>

class InputMemoryStream 
{
public:     
    InputMemoryStream(enet_uint8* buffer, size_t byteCount);
    ~InputMemoryStream();

    size_t GetRemainingDataSize() const;

    void Read(void* data, size_t byteCount);

    void Read(size_t& data);
    void Read(float& data);
    void Read(cocos2d::Vec2& data);

private:     
    enet_uint8* m_buffer;     
    size_t m_head;
    size_t m_capacity;
};