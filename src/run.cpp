#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "log.h"
#include "DB.h" 
#include "redis.h"
#include "connection.h"
#include "logic.h"

using namespace std;

int main(void)
{
	cout << "server is starting ...." << endl;

	Log::get_instance()->init();
	DB::get_instance()->InitDB();

	cout << "----------running---------"<<endl;
	while(!Exec())
	{
		//LOG_WARN("%s", "try restart connect");
		sleep(10);
	}

	return 0;

}