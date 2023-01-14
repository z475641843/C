#ifndef _INC_CEVENTBUFFERLIST
#define _INC_CEVENTBUFFERLIST

#include <stdio.h>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>

typedef struct tagEventBuffer
{
public:
    void *pBuffer;
    int s32BufLen;
	tagEventBuffer()
    {
        pBuffer = NULL;
        s32BufLen = 0;
    }
	tagEventBuffer(const struct tagEventBuffer &srcBuffer)
    {
        pBuffer = srcBuffer.pBuffer;
        s32BufLen = srcBuffer.s32BufLen;
    }
    ~tagEventBuffer()
    {
       
    }
} EventBuffer;

class CEventBufferList
{
    std::list<EventBuffer> m_listBuffer;
    std::mutex m_mtxLock;
    
public:
    CEventBufferList();
    ~CEventBufferList();
    int PopFront(EventBuffer &stEventBuffer);
    int PushBack(EventBuffer &stEventBuffer);
    int Size();
    void Clear();
};

#endif							/* _INC_CEVENTBUFFERLIST */
