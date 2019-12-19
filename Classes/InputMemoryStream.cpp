#include "InputMemoryStream.h"

InputMemoryStream::InputMemoryStream(enet_uint8* buffer, size_t byteCount) : m_buffer(buffer), m_head(0), m_capacity(byteCount)
{}


InputMemoryStream::~InputMemoryStream()
{
    std::free(m_buffer);
}


size_t InputMemoryStream::GetRemainingDataSize() const 
{ 
    return m_capacity - m_head; 
}


void InputMemoryStream::Read(void* data, size_t byteCount)
{
    std::memcpy(data, m_buffer + m_head, byteCount);
    m_head += byteCount;
}


void InputMemoryStream::Read(size_t& data) 
{ 
    Read(&data, sizeof(data)); 
}


void InputMemoryStream::Read(float& data)
{
    Read(&data, sizeof(data));
}


void InputMemoryStream::Read(cocos2d::Vec2& data) 
{ 
    Read(data.x);
    Read(data.y);
}