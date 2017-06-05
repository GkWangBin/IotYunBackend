#include "redis.h"
#include <iostream>
#include <string>

using namespace std;

int main(void){
	Redis redis;
	string command = "SISMEMBER dev wangbin";
	int result = -1;

	if(redis.ConnectRedis()){
		if (redis.CheckDeviceInCache("wangbin"))
		{
			result = redis.GetRedisInt();
		}
	}
	cout << "result: " << result << endl; 
	
	return 0;
}