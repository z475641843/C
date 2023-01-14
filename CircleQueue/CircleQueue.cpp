#include <iostream>
#include "CircleQueue.h"
#include "DataQueue.h"
#include "User.h"
using namespace xrs_circle_queue;

CQ_HANDLE CircleQueue_Create (CQ_PARAM_S *pstParam)
{
    DataQueue *pQueue = new DataQueue(pstParam->s32MaxSize, pstParam->s32MaxDuration, (pstParam->enMode == CQ_MODE_COPY));
    if (nullptr == pQueue)
    {
        return CQ_INVALID_HANDLE;
    }

    return (CQ_HANDLE)pQueue;
}

int CircleQueue_Destroy(CQ_HANDLE hCircleQueue)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    if (pQueue->IsBusy())
    {
        return XRS_CQ_ERR_BUSY;
    }
    pQueue->DeleteUser();
    delete pQueue;
    return XRS_CQ_OK; 
}

CQ_USER CircleQueue_AddUser(CQ_HANDLE hCircleQueue, CQ_USER_PARAM_S *pstUserParam)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    User *pUser =  pQueue->AddUser();
    pUser->Param() = *pstUserParam;
    return reinterpret_cast<CQ_USER>(pUser);
}

//!\brief删除消费者
int CircleQueue_DeleteUser(CQ_USER hQueueUser)
{
    if (hQueueUser == CQ_INVALID_USER)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    
    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    if (pUser->IsBusy())
    {
        return XRS_CQ_ERR_BUSY;
    }
    return pQueue->DeleteUser(pUser);
}

//!\brief写入数据到缓冲队列
int CircleQueue_AddData(CQ_HANDLE hQueue, CQ_DATA_S *pstData)
{
    if (hQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hQueue);
    const std::lock_guard<DataQueue> lock(*pQueue);
    return pQueue->AddNode(*pstData);
}

//!\brief从缓冲队列读取一个节点的数据信息，并将该节点的引用计数加1
int CircleQueue_GetData(CQ_USER hQueueUser, CQ_DATA_S *pstData)
{
    int32_t s32Result = 0;
    if (hQueueUser == CQ_INVALID_USER)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    
    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    s32Result = pQueue->GetNode(pUser, *pstData);
    return s32Result;
}

//!\brief将队列节点的引用计数减1，当节点的引用计数为0时，无消费者正在使用此节点
int CircleQueue_ReleaseData(CQ_USER hQueueUser, CQ_DATA_S *pstData)
{
    if (hQueueUser == CQ_INVALID_USER)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }

    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    pQueue->ReleaseNode(pUser);
    return XRS_CQ_OK;
}

//!\brief设置日志级别
void CircleQueue_SetLogLevel(int  s32Level)
{
}