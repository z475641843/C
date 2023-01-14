#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include "CircleQueue.h"

static CQ_MODE s_enCQMode = CQ_MODE_COPY;
int producer(CQ_HANDLE hQueue, const char *name, int32_t s32SleepTimeMs)
{
    CQ_DATA_S stData;
    std::fstream fs(name, std::ios::out|std::ios::trunc);
    int s32Channel = 0;
    while (true)
    {
        stData.s32Channel = (s32Channel%4);
        stData.s32Stream = 0;
        // 每50帧第1帧FrameType=0，后面49帧FrameType=1
        stData.s32FrameType = (((s32Channel/4)%50)?1:0);
        stData.s32DataSize = 100*1024;
        stData.pData = new uint8_t[stData.s32DataSize];
        int64_t s64Timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        stData.s64Timestamp = s64Timestamp;
        int32_t result = CircleQueue_AddData(hQueue, &stData);
        if (0 == result)
        {
            fs << "[" << stData.s32Seq << "]" << std::endl;
            fs << "pData=" << static_cast<void *>(stData.pData) << std::endl;
            fs << "Size=" << stData.s32DataSize << std::endl;
            fs << "Channel=" << stData.s32Channel << std::endl;
            fs << "Stream=" << stData.s32Stream << std::endl;
            fs << "FrameType=" << stData.s32FrameType << std::endl;
            fs << "hQueue=" << stData.hQueue << std::endl;
            //fs << "RefCount=" << stData.s32RefCount << std::endl;
            fs << "Timestamp=" << stData.s64Timestamp << std::endl;
            if (s_enCQMode == CQ_MODE_COPY)
            {
                delete [] stData.pData;
            }

        }
        else
        {
            delete [] stData.pData;
            std::cout << "CircleQueue_AddData failed\n" << std::endl;
        }
        stData.s32Stream = 1;
        stData.s32DataSize = 50*1024;
        stData.pData = new uint8_t[stData.s32DataSize];
        result = CircleQueue_AddData(hQueue, &stData);
        if (0 == result)
        {
            fs << "[" << stData.s32Seq << "]" << std::endl;
            fs << "pData=" << static_cast<void *>(stData.pData) << std::endl;
            fs << "Size=" << stData.s32DataSize << std::endl;
            fs << "Channel=" << stData.s32Channel << std::endl;
            fs << "Stream=" << stData.s32Stream << std::endl;
            fs << "FrameType=" << stData.s32FrameType << std::endl;
            fs << "hQueue=" << stData.hQueue << std::endl;
            //fs << "RefCount=" << stData.s32RefCount << std::endl;
            fs << "Timestamp=" << stData.s64Timestamp << std::endl;
            if (s_enCQMode == CQ_MODE_COPY)
            {
                delete [] stData.pData;
            }
        }
        else
        {
            delete [] stData.pData;
            std::cout << "CircleQueue_AddData failed\n" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        s32Channel++;
    }
    fs.close();
    return 0;
}

int user(CQ_USER hUser, const char *name, int32_t s32OccupyTimeMs, int32_t s32SleepTimeMs)
{
    CQ_DATA_S stData;
    int i = 1;

    std::fstream fs(name, std::ios::out|std::ios::trunc);
    while (true)
    {
        int32_t result = CircleQueue_GetData(hUser, &stData);
        if (0 == result)
        {
            fs << "[" << stData.s32Seq << "]" << std::endl;
            fs << "pData=" << static_cast<void *>(stData.pData) << std::endl;
            fs << "Size=" << stData.s32DataSize << std::endl;
            fs << "Channel=" << stData.s32Channel << std::endl;
            fs << "Stream=" << stData.s32Stream << std::endl;
            fs << "FrameType=" << stData.s32FrameType << std::endl;
            fs << "hQueue=" << stData.hQueue << std::endl;
            //fs << "RefCount=" << stData.s32RefCount << std::endl;
            fs << "Timestamp=" << stData.s64Timestamp << std::endl;
            if (0 == (i%100))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(s32OccupyTimeMs));
            }
            i++;
            CircleQueue_ReleaseData(hUser, &stData);
        }
        else
        {
            //std::cout << "CircleQueue_GetData failed" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(s32SleepTimeMs));
    }
    fs.close();
    return 0;
}

int testcase1(int argc, char *argv[])
{
    s_enCQMode = CQ_MODE_BYPASS;
    CQ_PARAM_S stCQParam;
    stCQParam.s32MaxSize = 1024*1024;
    stCQParam.s32MaxDuration = 1000;
    stCQParam.enMode = s_enCQMode;
    CQ_HANDLE hQueue = CircleQueue_Create(&stCQParam);
    CQ_USER_PARAM_S stUserParam = {0xff, 0xff};
    CQ_USER hUser1 = CircleQueue_AddUser(hQueue, &stUserParam);
    CQ_USER hUser2 = CircleQueue_AddUser(hQueue, &stUserParam);
    std::thread thread_producer(producer, hQueue, "producer.txt", 40);
    std::thread thread_user1(user, hUser1, "user1.txt", 10, 20);
    std::thread thread_user2(user, hUser2, "user2.txt", 10, 20);
    thread_producer.join();
    thread_user1.join();
    thread_user2.join();
    return 0;
}

int testcase2(int argc, char *argv[])
{
    s_enCQMode = CQ_MODE_COPY;
    CQ_PARAM_S stCQParam;
    stCQParam.s32MaxSize = 1024*1024;
    stCQParam.s32MaxDuration = 1000;
    stCQParam.enMode = s_enCQMode;
    CQ_HANDLE hQueue = CircleQueue_Create(&stCQParam);
    CQ_USER_PARAM_S stUserParam = {0xff, 0xff};
    CQ_USER hUser1 = CircleQueue_AddUser(hQueue, &stUserParam);
    CQ_USER hUser2 = CircleQueue_AddUser(hQueue, &stUserParam);
    std::thread thread_producer(producer, hQueue, "producer.txt", 40);
    std::thread thread_user1(user, hUser1, "user1.txt", 10, 20);
    std::thread thread_user2(user, hUser2, "user2.txt", 10, 20);
    thread_producer.join();
    thread_user1.join();
    thread_user2.join();
    return 0;
}

int testcase3(int argc, char *argv[])
{
    s_enCQMode = CQ_MODE_COPY;
    CQ_PARAM_S stCQParam;
    stCQParam.s32MaxSize = 1024*1024;
    stCQParam.s32MaxDuration = 1000;
    stCQParam.enMode = s_enCQMode;
    CQ_HANDLE hQueue = CircleQueue_Create(&stCQParam);
    CQ_USER_PARAM_S stUserParam = {0xff, 0xff};
    CQ_USER hUser1 = CircleQueue_AddUser(hQueue, &stUserParam);
    CQ_USER hUser2 = CircleQueue_AddUser(hQueue, &stUserParam);
    std::thread thread_producer(producer, hQueue, "producer.txt", 40);
    std::thread thread_user1(user, hUser1, "user1.txt", 10, 20);
    std::thread thread_user2(user, hUser2, "user2.txt", 1000, 20);
    thread_producer.join();
    thread_user1.join();
    thread_user2.join();
    return 0;
}

int testcase4(int argc, char *argv[])
{
    s_enCQMode = CQ_MODE_COPY;
    CQ_PARAM_S stCQParam;
    stCQParam.s32MaxSize = 1024*1024;
    stCQParam.s32MaxDuration = 1000;
    stCQParam.enMode = s_enCQMode;
    CQ_HANDLE hQueue = CircleQueue_Create(&stCQParam);
    CQ_USER_PARAM_S stUserParam = {0xff, 0xff};
    CQ_USER hUser1 = CircleQueue_AddUser(hQueue, &stUserParam);
    CQ_USER hUser2 = CircleQueue_AddUser(hQueue, &stUserParam);
    std::thread thread_producer(producer, hQueue, "producer.txt", 40);
    std::thread thread_user1(user, hUser1, "user1.txt", 10, 20);
    std::thread thread_user2(user, hUser2, "user2.txt", 1000, 100);
    thread_producer.join();
    thread_user1.join();
    thread_user2.join();
    return 0;
}

int testcase5(int argc, char *argv[])
{
    s_enCQMode = CQ_MODE_BYPASS;
    CQ_PARAM_S stCQParam;
    stCQParam.s32MaxSize = 2*1024*1024;
    stCQParam.s32MaxDuration = 1000;
    stCQParam.enMode = s_enCQMode;
    CQ_HANDLE hQueue = CircleQueue_Create(&stCQParam);
    CQ_USER_PARAM_S stUserParam = {0, 0};
    CQ_USER hUser0_0 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {0,1};
    CQ_USER hUser0_1 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {1,0};
    CQ_USER hUser1_0 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {1,1};
    CQ_USER hUser1_1 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {2,0};
    CQ_USER hUser2_0 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {2,1};
    CQ_USER hUser2_1 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {3,0};
    CQ_USER hUser3_0 = CircleQueue_AddUser(hQueue, &stUserParam);
    stUserParam = {3,1};
    CQ_USER hUser3_1 = CircleQueue_AddUser(hQueue, &stUserParam);
    std::thread thread_producer(producer, hQueue, "producer.txt", 40);
    std::thread thread_user1(user, hUser0_0, "user0-0.txt", 10, 20);
    std::thread thread_user2(user, hUser0_1, "user0-1.txt", 10, 20);
    std::thread thread_user3(user, hUser1_0, "user1-0.txt", 10, 20);
    std::thread thread_user4(user, hUser1_1, "user1-1.txt", 10, 20);
    std::thread thread_user5(user, hUser2_0, "user2-0.txt", 10, 20);
    std::thread thread_user6(user, hUser2_1, "user2-1.txt", 10, 20);
    std::thread thread_user7(user, hUser3_0, "user3-0.txt", 10, 20);
    std::thread thread_user8(user, hUser3_1, "user3-1.txt", 10, 20);
    thread_producer.join();
    thread_user1.join();
    thread_user2.join();
    thread_user3.join();
    thread_user4.join();
    thread_user5.join();
    thread_user6.join();
    thread_user7.join();
    thread_user8.join();
    return 0;
}

//#define TEST_CASE_1
//#define TEST_CASE_2
//#define TEST_CASE_3
//#define TEST_CASE_4
#define TEST_CASE_5

int main(int argc, char *argv[])
{
#if defined(TEST_CASE_1)
    testcase1(argc, argv);
#elif defined(TEST_CASE_2)
    testcase2(argc, argv);
#elif defined(TEST_CASE_3)
    testcase3(argc, argv);
#elif defined(TEST_CASE_4)
    testcase4(argc, argv);
#elif defined(TEST_CASE_5)
    testcase5(argc, argv);
#endif
    return 0;
}