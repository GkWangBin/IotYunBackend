#include "log.h"
#include <unistd.h>
#include <pthread.h>

void *f(void* args)
{
	for(int i = 0;i < 1000; i++)
	{
			Log::get_instance()->write_log(1, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);
			Log::get_instance()->write_log(2, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);
			Log::get_instance()->write_log(3, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);

			LOG_INFO("%d", 123456789);
			LOG_ERROR("%d", 123456789);
			LOG_DEBUG("%d", 123456789);
			LOG_WARN("%d", 123456789);

	}
	pthread_exit(NULL);
}

int main()
{
	//限制每条信息长100字符，每个日志文件中最多会打200w条信息
	Log::get_instance()->init("./log/mylog.log", 100, 2000000);
	int i = 0;
	Log::get_instance()->write_log(1, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);
	Log::get_instance()->write_log(2, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);
	Log::get_instance()->write_log(3, "d=%d,c=%c,s=%s,f=%f", i,'a',"log", 1.000);

	LOG_INFO("%d", 123456789);
	LOG_ERROR("%d", 123456789);
	LOG_DEBUG("%d", 123456789);
	LOG_WARN("%d", 123456789);

	pthread_t id;
	for(int i = 0; i < 10; i++)
	{
		pthread_create(&id, NULL, f, NULL);
		pthread_join(id,NULL);
	}

	for(;;)
	{
		sleep(5);
		Log::get_instance()->flush();
	}
	pthread_exit(NULL);

	return 0;
}
