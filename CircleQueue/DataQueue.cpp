#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include "CircleQueue.h"
#include "DataQueue.h"
#include "User.h"
#define CQ_DEBUG //printf
#define CQ_INFO //printf
#define CQ_ERR printf
namespace xrs_circle_queue {
DataNode::DataNode(DataQueue *ptrQueue, CQ_DATA_S &stData, bool bCopy):
    s32Channel(stData.s32Channel), s32Stream(stData.s32Stream), s32FrameType(stData.s32FrameType),
    pData(stData.pData),s32DataSize(stData.s32DataSize),
    s64Timestamp(stData.s64Timestamp),
    pQueue(ptrQueue),
    s32Seq(stData.s32Seq),
    s32RefCount(0)
{
    tpSteady = std::chrono::steady_clock::now();
    if (bCopy)
    {
        pData = (uint8_t *)malloc(stData.s32DataSize);
        memcpy(pData, stData.pData, stData.s32DataSize);
    }
}

DataNode &DataNode::operator=(const DataNode &refNode)
{
    s32Channel = refNode.s32Channel;
    s32Stream = refNode.s32Stream;
    s32FrameType = refNode.s32FrameType;
    pData = refNode.pData;
    s32DataSize = refNode.s32DataSize;
    s64Timestamp = refNode.s64Timestamp;
    tpSteady = refNode.tpSteady;
    pQueue = refNode.pQueue;
    s32Seq = refNode.s32Seq;
    s32RefCount = refNode.s32RefCount;
    return *this;
}

DataNode::operator CQ_DATA_S() const
{
    CQ_DATA_S stData;
    stData.s32Channel = s32Channel;
    stData.s32Stream = s32Stream;
    stData.s32FrameType = s32FrameType;
    stData.pData = pData;
    stData.s32DataSize = s32DataSize;
    stData.s64Timestamp = s64Timestamp;
    stData.hQueue = reinterpret_cast<CQ_HANDLE>(pQueue);
    stData.s32Seq = s32Seq;
    stData.s32RefCount = s32RefCount;
    return stData;
}

DataQueue::DataQueue(int s32Size, int s32Duration, bool bCopyMode):
    m_s32TotalSpace(s32Size), m_s32MaxDuration(s32Duration), 
    m_bCopyMode(bCopyMode)
{
    m_listQueue.clear();
    m_listExpired.clear();
    m_objStats.m_s32SeqHead = 0;
    m_objStats.m_s32SeqTail = 0;
    m_objStats.m_s32TotalAddNodes = 0;
    m_objStats.m_s32TotalNodeSize = 0;
    m_objStats.m_s32UsedSpace = 0;
    m_objStats.m_timeHead = std::chrono::_V2::steady_clock::now();
    m_objStats.m_timeTail = m_objStats.m_timeHead;
}

DataQueue::~DataQueue()
{
}

int32_t DataQueue::AddNode(CQ_DATA_S &stData)
{
    if (RequestSpace(stData.s32DataSize))
    {
        CQ_DEBUG("Queue %p request space %d failed\n", this, stData.s32DataSize);
        return XRS_CQ_ERR_OOM;
    }

    stData.s32Seq = (m_objStats.m_s32SeqTail+1);
    stData.s32RefCount = 0;
    stData.hQueue = reinterpret_cast<CQ_HANDLE>(this);
    CQ_DEBUG("Queue %p AddData:pData=%p, size=%d, seq=%d, timestamp=%lld\n",
        this,
        stData.pData, stData.s32DataSize, 
        stData.s32Seq, stData.s64Timestamp);

    DataNode nodeNew(this, stData, this->m_bCopyMode);
    if (m_listQueue.empty())
    {
        m_objStats.m_s32SeqHead = stData.s32Seq;
        m_objStats.m_timeHead = nodeNew.Time();
    }
    m_listQueue.push_back(nodeNew);
    m_objStats.m_s32SeqTail = stData.s32Seq;
    m_objStats.m_timeTail = nodeNew.Time();
    m_objStats.m_s32TotalAddNodes++;
    m_objStats.m_s32TotalNodeSize += stData.s32DataSize;
    m_objStats.m_s32UsedSpace += stData.s32DataSize;
    CQ_DEBUG("m_objStats:\n");
    CQ_DEBUG("Head={%d},Tai={%d},duration=%lld\n", m_objStats.m_s32SeqHead, m_objStats.m_s32SeqTail,
        std::chrono::duration_cast<std::chrono::milliseconds>(m_objStats.m_timeTail-m_objStats.m_timeHead).count());
    CQ_DEBUG("TotalAddNodes=%d, TotalNodeSize=%d, UsedSpace=%d\n",
        m_objStats.m_s32TotalAddNodes, m_objStats.m_s32TotalNodeSize, m_objStats.m_s32UsedSpace);
    return XRS_CQ_OK;
}

int32_t DataQueue::GetNextNode(User *pUser, CQ_DATA_S &refData)
{
    if (m_listQueue.empty())
    {
        CQ_DEBUG("Queue %p User %p read failed,  Queue is empty\n", this, pUser);
        return XRS_CQ_ERR_EMPTY;
    }
    DataQueue::NodeList::iterator itNode;
    // 第一次获取节点,则直接读最后一个节点（最新的数据）
    if (pUser->Seq() < 0)
    {
        itNode = m_listQueue.end();
        itNode--;
    }
    // 头节点的序号比用户最后一次读的节点大，直接读头节点
    else if (m_listQueue.begin()->Seq() > pUser->Seq())
    {
        itNode = m_listQueue.begin();
    }
    // 正常往后读取下一个节点
    else
    {
        itNode = pUser->Node();
        itNode++;
        // 没有下一个节点了
        if (itNode == m_listQueue.end())
        {
            CQ_DEBUG("Queue %p User %p read reach tail\n", this, pUser);
            return XRS_CQ_ERR_TAIL;
        }
    }
    itNode->AddRef();
    pUser->GetNode(itNode->Seq(), itNode);
    refData = *itNode;
    CQ_DEBUG("Queue %p User %p ReadData: pData=%p, size=%d, seq=%d, timestamp=%lld\n",
        this, pUser,
        refData.pData, refData.s32DataSize, 
        refData.s32Seq, refData.s64Timestamp);

    return XRS_CQ_OK;
}

int32_t DataQueue::GetNode(User *pUser, CQ_DATA_S &refData)
{
    int32_t s32Ret = 0;
    bool bCheckChannel = (0xff != pUser->Param().s32Channel);
    bool bCheckStream = (0xff != pUser->Param().s32Stream);
    while (0 == (s32Ret = GetNextNode(pUser, refData)))
    {
        if (((!bCheckChannel) || (refData.s32Channel == pUser->Param().s32Channel)) &&
            ((!bCheckStream) || (refData.s32Stream == pUser->Param().s32Stream)))
        {
            return XRS_CQ_OK;
        }
        ReleaseNode(pUser);
    }
    return s32Ret;
}

int32_t DataQueue::ReleaseNode(User *pUser)
{
    NodeList::iterator itFind = FindNode(pUser->Seq(), pUser->Node());
    if (itFind == m_listQueue.end())
    {
        CQ_DEBUG("Queue %p User %p find node %d failed, impossible!!\n", this, pUser, pUser->Seq());
        return XRS_CQ_ERR_INVALID_PARAM;
    }
    // 引用计数递减
    itFind->Release(); 
    pUser->ReleaseNode();

    if (IsExpired(pUser->Seq()))
    {
        // 如果是已经超时的节点，并且无人占用，释放内存，并从超时队列中移除，更新总消耗内存
        if (!itFind->IsBusy())
        {
            CQ_DEBUG("Queue %p User %p free expired node %d size %d !\n", this, pUser, itFind->Seq(), itFind->Size());
            itFind->FreeMem();
            m_listExpired.erase(itFind);
            m_objStats.m_s32UsedSpace -= itFind->Size();
        }
    }
    CQ_DEBUG("Queue %p User %p release node %d\n", this, pUser, pUser->Seq());
    // 正常队列中的节点只减引用计数，不释放内存
    return XRS_CQ_OK;
}

bool DataQueue::IsBusy()
{
    // 正常队列中有一个节点被占用，则整个队列繁忙
    // 超时队列不为空，则队列繁忙
    for (auto it = m_listQueue.begin(); it != m_listQueue.end(); it++)
    {
        if (it->IsBusy())
        {
            return true;
        }
    }
    return !m_listExpired.empty();
}


int DataQueue::RequestSpace(int32_t s32Size)
{
    // 如果内存不能得到满足，则一直回收头节点（头节点回收后，下个节点就变成了头节点）
    while ((m_s32TotalSpace-m_objStats.m_s32UsedSpace) < s32Size)
    {
        // 队列为空了还不能满足(不太可能出现)，则应该是出错了
        if (m_listQueue.empty())
        {
            CQ_ERR("Queue %p is empty, but request space %d failed, TotalSpace=%d, UsedSpace=%d !!\n", 
                this, s32Size, m_s32TotalSpace, m_objStats.m_s32UsedSpace);
            // 无法回收到所需要的内存
            return XRS_CQ_ERR_OOM;
        }
        auto itNode = m_listQueue.begin();
        // 头节点被占用，放入超时队列
        if (itNode->IsBusy())
        {
            m_listExpired.push_back(*itNode);
        }
        // 未被占用，则直接释放内存
        else
        {
            itNode->FreeMem();
            CQ_DEBUG("Queue %p free node %d size %d !!\n", this, itNode->Seq(), itNode->Size());
            m_objStats.m_s32UsedSpace -= itNode->Size();
        }
        // 头节点从队列中删除
        m_listQueue.erase(itNode);
    }
    if (!m_listQueue.empty())
    {
        auto itNode = m_listQueue.begin();
        m_objStats.m_s32SeqHead = itNode->Seq();
        m_objStats.m_timeHead = itNode->Time();
    }
    return XRS_CQ_OK;
}

DataQueue::NodeList::iterator DataQueue::FindNode(int32_t s32Seq, NodeList::iterator itNode)
{
    // internal method, do not check parameters
    // search expire list first, because it is very short
    for (auto it = m_listExpired.begin(); it != m_listExpired.end(); it++)
    {
        if (it->Seq() == s32Seq)
        {
            return it;
        }
    }

    return itNode;
}

bool DataQueue::IsExpired(int32_t s32Seq)
{
    // internal method, do not check parameters
    if (m_listQueue.empty())
    {
        return true;
    }
    if (m_listQueue.begin()->Seq() > s32Seq)
    {
        return true;
    }

    return false;
}

User * DataQueue::AddUser()
{
    if (m_vecUser.size() > CQ_MAX_USER)
    {
        // 达到最大的消费者数
        return nullptr;
    }
    User *pUser = new User(this);
    if (nullptr == pUser)
    {
        // oom
        return nullptr;
    }
    m_vecUser.push_back(pUser);
    CQ_INFO("Queue %p add user %p !!\n", this, pUser);
    return pUser;
}

int32_t DataQueue::DeleteUser(User *pUser)
{
    if (nullptr == pUser)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }
    for (auto it = m_vecUser.begin(); it != m_vecUser.end(); ) 
    {
        if (*it == pUser)
        {
            it = m_vecUser.erase(it);
            CQ_INFO("Queue %p delete user %p !!\n", this, pUser);
            delete pUser;
            return XRS_CQ_OK;
        } else {
            ++it;
        }
    }

    return XRS_CQ_ERR_INVALID_USER;
}

int32_t DataQueue::DeleteUser(void)
{
    if (m_vecUser.size() >= CQ_MAX_USER)
    {
        // 达到最大的消费者数
        return XRS_CQ_ERR_TOOMUCH_USER;
    }
    for (auto it = m_vecUser.begin(); it != m_vecUser.end(); ) 
    {
        delete (*it);
        it = m_vecUser.erase(it);
    }

    return XRS_CQ_OK;
}


}