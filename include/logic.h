/**
 * author: wangbin (gkwangbin@126.com)
 * date: 2017-05-18
 */

#ifndef LOGIC_H
#define LOGIC_H

#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>
#include <time.h>

#define INFO_MAX_LEN 	256
#define AUTH_CHECK_LEN  16
#define SECRET_KEY      "3b6a04d6c38ac0c16c483117d4ad59af"

#define SQL_SELECT      "select devid from dev_t where devid=%s and flag=1;"
#define SQL_INSERT		"insert into info_t_%d (devid, info, ctime, flag) values (%s, \"%s\", %d, %d);"
//errno
#define ERR_SUCCESS   	0
#define ERR_ILLEGALLY	1
#define ERR_NODEVICE  	2

using namespace std;

struct sensor_s{
	string 	devId;
	//char*   auth[AUTH_CHECK];
	string 	info;
	int 	ctime;
	int 	flag;
};

/**
 * [ProcessData function.cpp的调用接口]
 * @param  recv [description]
 * @return      [description]
 */
int ProcessData(const char * recv);

#endif