/**
 * author:wangbin(gkwangbin@126.com)
 * date: 2017-05-17
 * desc: redis model
 */
#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <string>

using namespace std;

#define TIMEOUT 2
#define REDIS_HOST "127.0.0.1"
#define REDIS_PORT 6379

#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

#define CHECK_COMMAND "SISMEMBER dev %s"
#define SADD_COMMAND "SADD dev %s"

class Redis{
public:
	Redis();
	~Redis();
	bool ConnectRedis();
	bool SetIsMember(string devName);
	bool SetAdd(string devName);
	int GetRedisInt();

private:
	redisContext* context;
	redisReply*   reply;
	struct timeval timeout;
	int redisInt = -1;
};

#endif