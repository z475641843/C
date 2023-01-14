#include "CEventBufferList.h"

CEventBufferList::CEventBufferList()
{
	
}

CEventBufferList::~CEventBufferList()
{
	Clear();
}

int CEventBufferList::PushBack(EventBuffer &stEventBuffer)
{
    std::lock_guard<std::mutex> gaurd(m_mtxLock);
    m_listBuffer.push_back(stEventBuffer);
    return 0;
}

int CEventBufferList::PopFront(EventBuffer &stEventBuffer)
{
    std::lock_guard<std::mutex> gaurd(m_mtxLock);
    if (m_listBuffer.empty())
    {
        return -1;
    }
    stEventBuffer = m_listBuffer.front();
    m_listBuffer.pop_front();
    return 0;
}

int CEventBufferList::Size()
{
    std::lock_guard<std::mutex> gaurd(m_mtxLock);
    return m_listBuffer.size();
}

void CEventBufferList::Clear()
{
    while (1)
    {
        std::lock_guard<std::mutex> gaurd(m_mtxLock);
        if (m_listBuffer.size())
        {
            m_listBuffer.pop_front();
        }
        else
        {
            break;
        }
    }
};

