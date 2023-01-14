#ifndef __XRS_CIRCLE_QUEUE_USER_H_
#define __XRS_CIRCLE_QUEUE_USER_H_
#include <vector>
#include "CircleQueue.h"
#include "DataQueue.h"

namespace xrs_circle_queue {

class User
{
private:
    DataQueue *m_queueData;
    int32_t m_s32SeqGot;
    int32_t m_s32SeqLast;
    DataQueue::NodeList::iterator m_itProgress;
    CQ_USER_PARAM_S m_strParam;
public:
    User(DataQueue *pQueue): 
        m_queueData(pQueue),
        m_s32SeqGot(-1),
        m_s32SeqLast(-1)
    {
        m_strParam.s32Channel = 0xff;
        m_strParam.s32Stream = 0xff;
    };
    User(User &refUser):
        m_queueData(refUser.m_queueData),m_s32SeqGot(refUser.m_s32SeqGot)
    {
        m_strParam = refUser.m_strParam;
    };
    CQ_USER_PARAM_S & Param() 
    {
        return m_strParam;
    };
    ~User() {};
    bool IsBusy() { return (m_s32SeqGot != -1); };
    int32_t GetNode(int32_t s32Node, DataQueue::NodeList::iterator itNode);
    int32_t ReleaseNode();
    DataQueue::NodeList::iterator Node() { return m_itProgress; };
    int32_t Seq() { return m_s32SeqLast; };
    DataQueue *Queue() { return m_queueData; };
};
}
#endif

