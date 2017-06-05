#include <string>
#include <iostream>
#include "redis.h"
#include "log.h"

using namespace std;

Redis::Redis(){
    timeout = {TIMEOUT, 0};
}

Redis::~Redis(){
    if(reply != NULL){
        redisFree(context);
    }
}

/**
 * [Redis::connectRedis description]
 * @return [description]
 */
bool Redis::ConnectRedis(){
    if(context != NULL){
        //redisFree(context);
    }
    context = (redisContext*)redisConnectWithTimeout(REDIS_HOST, REDIS_PORT, timeout);
    if ( (NULL == context) || (context->err) )
        {
            if (context)
            {
                //std::cout << "connect redis error:" << context->errstr << std::endl;
                LOG_ERROR("%s,%s", "connect redis error:", context->errstr);
            }
            else
            {
                //std::cout << "connect error: can't allocate redis context." << std::endl;
                LOG_ERROR("%s", "connect error: can't allocate redis context.");               
            }
            return false;
        }
    return true;
}

/**
 * [Redis::getRedisType description]
 * @return [description]
 */
int Redis::GetRedisInt(){
    return redisInt;
}

/**
 * [SetIsmember description]
 * @param  command [description]
 * @return         [description]
 * 一般情况下只执行查询元素知否在集合中，或者将元素插入到集合中
 */
bool Redis::SetIsMember(string devName){
    reply = (redisReply*)redisCommand(context, CHECK_COMMAND, devName.c_str());
    if(reply == NULL){
        LOG_WARN("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "comand SISMEMBER error!");
        return false;
    }
    
    if((int)(reply->type) == REDIS_REPLY_INTEGER)
    {
        redisInt = (int)(reply->integer);
        freeReplyObject(reply);
        //LOG_INFO("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "Is Set member!");

        return true;
    }
    else{
        LOG_WARN("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "redis reply data error!");
        //LOG
        freeReplyObject(reply);
        return false;
    }
}

/**
 * [Redis::SetAdd description]
 * @param  devNames [description]
 * @return          [description]
 */
bool Redis::SetAdd(string devName){
    reply = (redisReply*)redisCommand(context, SADD_COMMAND, devName.c_str());
    if(reply == NULL){
        LOG_WARN("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "command SADD error!");
        return false;
    }
    
    if((int)(reply->type) == REDIS_REPLY_INTEGER)
    {
        redisInt = (int)(reply->integer);
        freeReplyObject(reply);
        LOG_INFO("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "SADD secccess!");

        return true;
    }
    else{
        LOG_WARN("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "redis reply data error!");
        freeReplyObject(reply);

        return false;
    }    
}




