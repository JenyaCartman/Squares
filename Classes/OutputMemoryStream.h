#pragma once

#include <enet/enet.h>
#include <cocos2d.h>
#include <cstdlib>

class OutputMemoryStream 
{
public:     
    OutputMemoryStream();
    ~OutputMemoryStream();
            
    enet_uint8* GetBufferPtr() const;
    size_t GetLength() const;
            
    void Write(const void* data, size_t byteCount);

    void Write(size_t data);
    void Write(float data);
    void Write(const cocos2d::Vec2& data);

private:     
    void ReallocBuffer(size_t newLength);

private:
    enet_uint8* m_buffer;
    size_t m_head;     
    size_t m_capacity;
};