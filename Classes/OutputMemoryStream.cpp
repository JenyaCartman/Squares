#include "OutputMemoryStream.h"
#include <array>
   
OutputMemoryStream::OutputMemoryStream() : m_buffer(nullptr), m_head(0), m_capacity(0) 
{ 
    ReallocBuffer(32); 
}


OutputMemoryStream::~OutputMemoryStream()
{ 
    std::free(m_buffer); 
}
            

enet_uint8* OutputMemoryStream::GetBufferPtr() const
{ 
    return m_buffer; 
}


size_t OutputMemoryStream::GetLength() const 
{ 
    return m_head; 
}

            
void OutputMemoryStream::Write(const void* data, size_t byteCount)
{
    size_t resultHead = m_head + byteCount;
    if (resultHead > m_capacity)
    {
        ReallocBuffer((m_capacity * 2 > resultHead) ? m_capacity * 2 : resultHead);
    }
   
    std::memcpy(m_buffer + m_head, data, byteCount);  
    m_head = resultHead;  
}


void OutputMemoryStream::Write(size_t data) 
{ 
    Write(&data, sizeof(data)); 
}


void OutputMemoryStream::Write(float data)
{
    Write(&data, sizeof(data));
}


void OutputMemoryStream::Write(const cocos2d::Vec2& data)
{
    Write(data.x);
    Write(data.y);
}

   
void OutputMemoryStream::ReallocBuffer(size_t newLength)
{
    m_buffer = static_cast<enet_uint8*>(std::realloc(m_buffer, newLength));
    if (m_buffer == nullptr)
    {
        OutputDebugStringA("Realloc buffer failed.\n");
    }
    m_capacity = newLength;
} 