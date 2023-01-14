#ifndef __XRS_CIRCLE_QUEUE_H_
#define __XRS_CIRCLE_QUEUE_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

#define CQ_LOG_DEBUG (0)
#define CQ_LOG_INFO (1)
#define CQ_LOG_WARNING (2)
#define CQ_LOG_ERR (3)
#define CQ_LOG_EMERG (4)

#define XRS_CQ_MODULE_ID (19000)
#define XRS_CQ_OK                   (0)	
#define XRS_CQ_ERR_INVALID_HANDLE   (XRS_CQ_MODULE_ID+1)		//缓冲队列句柄无效
#define XRS_CQ_ERR_INVALID_USER     (XRS_CQ_MODULE_ID+2)		//缓冲队列消费者句柄无效
#define XRS_CQ_ERR_TOOMUCH_USER     (XRS_CQ_MODULE_ID+3)		//缓冲队列消费者达到上限
#define XRS_CQ_ERR_INVALID_PARAM    (XRS_CQ_MODULE_ID+4)		//缓冲队列参数错误
#define XRS_CQ_ERR_BUSY             (XRS_CQ_MODULE_ID+5)		//缓冲队列繁忙
#define XRS_CQ_ERR_USER_BUSY        (XRS_CQ_MODULE_ID+6)        //缓冲队列消费者繁忙
#define XRS_CQ_ERR_EMPTY            (XRS_CQ_MODULE_ID+7)        //缓冲队列为空
#define XRS_CQ_ERR_NOSUCH_DATA      (XRS_CQ_MODULE_ID+8)        //缓冲对接节点不存在
#define XRS_CQ_ERR_OOM              (XRS_CQ_MODULE_ID+9)		//内存分配失败或内存不足
#define XRS_CQ_ERR_TAIL             (XRS_CQ_MODULE_ID+10)       //已经读取到缓冲区结尾

#define CQ_INVALID_HANDLE ((uint32_t)-1)
#define CQ_INVALID_USER ((uint32_t)-1)
#define CQ_MAX_USER (16)
typedef uint32_t CQ_HANDLE;
typedef uint32_t CQ_USER;

typedef enum {
    // 缓冲区直接进入队列
    CQ_MODE_BYPASS,
    // 队列自行分配新的缓冲区
    CQ_MODE_COPY,
} CQ_MODE;

typedef void CQ_Callback_BufferFree(void *);

//!\brief 定义缓冲队列参数结构体
typedef struct xrsCQ_PARAM_S
{
    //!\brief s32MaxDuration	缓冲队列的缓冲的最大时间长度
    int32_t s32MaxDuration;
    //!\brief s32MaxSize	缓冲队列的最大占用内存
    int32_t s32MaxSize;
    //!\brief s32WorkMode CQ_MODE_BYPASS-拷贝模式 CQ_MODE_COPY-无拷贝模式
    CQ_MODE enMode;
} CQ_PARAM_S;

//!\brief 定义缓冲队列参数结构体
typedef struct xrsCQ_USER_PARAM_S
{
    //!\brief s32Channel	用户关联的通道，0xff表示所有通道
    int32_t s32Channel;
    //!\brief s32Stream	用户关联的码流，0xff表示所有码流
    int32_t s32Stream;
} CQ_USER_PARAM_S;

//!\brief 定义缓冲队列节点结构体
typedef struct xrsCQ_DATA_S
{
    //!\brief s32Channel 通道
    int32_t s32Channel;
    //!\brief s32Stream	码流
    int32_t s32Stream;
    //!\brief s32FrameType 帧类型
    int32_t s32FrameType;
    //!\brief 内存块地址
    uint8_t *pData;
    //!\brief 内存块大小
    int32_t s32DataSize;
    //!\brief 系统时间戳,unix timestamp,微秒数
    int64_t s64Timestamp;
    //!\brief 队列句柄
    CQ_HANDLE hQueue;
    //!\brief 序号
    int32_t s32Seq;
    //!\brief 引用计数
    int32_t s32RefCount;
} CQ_DATA_S;

//!\brief//!\brief设置循环队列属性，创建循环队列
CQ_HANDLE CircleQueue_Create (CQ_PARAM_S *pstParam);

//!\brief销毁循环队列
int32_t CircleQueue_Destroy(CQ_HANDLE hCircleQueue);

//!\brief添加消费者
CQ_USER CircleQueue_AddUser(CQ_HANDLE hCircleQueue, CQ_USER_PARAM_S *pstUserParam);

//!\brief删除消费者
int32_t CircleQueue_DeleteUser(CQ_USER hQueueUser);

//!\brief写入数据到缓冲队列
int32_t CircleQueue_AddData(CQ_HANDLE hQueue, CQ_DATA_S *pstData);

//!\brief从缓冲队列读取一个节点的数据信息，并将该节点的引用计数加1
int32_t CircleQueue_GetData(CQ_USER hQueueUser, CQ_DATA_S *pstData);

//!\brief将队列节点的引用计数减1，当节点的引用计数为0时，无消费者正在使用此节点
int32_t CircleQueue_ReleaseData(CQ_USER hQueueUser, CQ_DATA_S *pstData);

//!\brief设置日志级别
void CircleQueue_SetLogLevel(int  s32Level);

#if defined(__cplusplus)
}
#endif

#endif
