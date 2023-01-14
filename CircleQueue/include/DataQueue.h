#ifndef __XRS_DATA_QUEUE_H_
#define __XRS_DATA_QUEUE_H_
#include <stdint.h>
#include <list>
#include <ctime>
#include <chrono>
#include <mutex>
#include <vector>

#include "CircleQueue.h"

using namespace std;

namespace xrs_circle_queue {
class User;
class DataQueue;
class DataNode
{
private:
    //!\brief 通道号
    int32_t s32Channel;
    //!\brief 码流
    int32_t s32Stream;
    //!\brief s32FrameType 帧类型
    int32_t s32FrameType;
    //!\brief 内存块地址
    uint8_t *pData;
    //!\brief 内存块大小
    int32_t s32DataSize;
    //!\brief 系统时间戳,unix timestamp
    int64_t s64Timestamp;
    //!\brief 时间戳，系统启动开始计时的绝对时间戳
    std::chrono::steady_clock::time_point tpSteady;
    //!\brief 队列句柄
    DataQueue *pQueue;
    //!\brief 序号
    int32_t s32Seq;
    //!\brief 引用计数
    int32_t s32RefCount;
public:
    DataNode(DataQueue * ptrQueue, uint8_t *pBuffer, int32_t s32Size):
        pData(pBuffer), s32DataSize(s32Size),pQueue(ptrQueue) {};
    DataNode(DataQueue *ptrQueue, CQ_DATA_S &stData, bool bCopy);
    ~DataNode() {};
    DataNode &operator=(const DataNode &refNode);
    operator CQ_DATA_S() const;
    bool IsBusy() const { return (s32RefCount > 0); };
    void AddRef() { s32RefCount++; };
    void Release() { (s32RefCount > 0)?s32RefCount--:0; };
    void FreeMem() { if (pData) {delete pData; pData = nullptr;};};
    int32_t Size() const { return s32DataSize; };
    int32_t Seq() const { return s32Seq; };
    std::chrono::steady_clock::time_point Time() const { return tpSteady; };
    int Check(CQ_USER_PARAM_S &refParam) 
    { 
        return ((s32Channel == refParam.s32Channel) && 
            (s32Stream == refParam.s32Stream));
    };
};

struct QueueStatus
{
    int32_t m_s32TotalAddNodes;
    int32_t m_s32TotalNodeSize;
    int32_t  m_s32UsedSpace;
    std::chrono::time_point<std::chrono::steady_clock> m_timeHead;
    std::chrono::time_point<std::chrono::steady_clock> m_timeTail;
    int32_t  m_s32SeqHead;
    int32_t  m_s32SeqTail;
};


class DataQueue
{
public:
    typedef list<DataNode> NodeList;
    DataQueue(int32_t s32Size, int32_t s32Duration, bool bCopyMode);
    ~DataQueue();
    int32_t AddNode(CQ_DATA_S &stData);
    int32_t GetNode(User *pUser, CQ_DATA_S &refData);
    int32_t ReleaseNode(User *pUser);
    bool IsBusy();
    User *AddUser(void);
    int32_t DeleteUser(User *pUser);
    int32_t DeleteUser(void);
    void lock() {m_mtxLock.lock();};
    void unlock() {m_mtxLock.unlock();};

private:
    int RequestSpace(int32_t s32Size);
    DataQueue::NodeList::iterator FindNode(int32_t s32Seq, NodeList::iterator itNode);
    bool IsExpired(int32_t s32Seq);
    int32_t GetNextNode(User *pUser, CQ_DATA_S &refData);

private:
    int32_t  m_s32TotalSpace;
    int32_t  m_s32MaxDuration;
    bool m_bCopyMode;
    NodeList m_listQueue;
    NodeList m_listExpired;
    QueueStatus m_objStats;
    std::mutex m_mtxLock;

    typedef std::vector<User *> UserVector;
    UserVector m_vecUser;
};

}
#endif