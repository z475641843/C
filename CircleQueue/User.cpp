#include <iostream>
#include "CircleQueue.h"
#include "User.h"
namespace xrs_circle_queue{
int32_t User::GetNode(int32_t s32NodeSeq, DataQueue::NodeList::iterator itNode)
{
    if (m_s32SeqGot >= 0)
    {
        // 不能同时占用两个节点
        return XRS_CQ_ERR_USER_BUSY;
    }
    m_s32SeqGot = s32NodeSeq;
    m_s32SeqLast = s32NodeSeq;
    m_itProgress = itNode;
    return XRS_CQ_OK;
}

int32_t User::ReleaseNode()
{
    m_s32SeqGot = -1;
    return XRS_CQ_OK;
}

}
