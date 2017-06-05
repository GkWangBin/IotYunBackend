/**
 * anthor: wangbin(gkwangbin@126.com)
 * date: 2017-05-17
 * desc:db model
 */

#ifndef  DB_H  
#define  DB_H

#include <iostream>
#include <string>  
#include <mysql/mysql.h>  

//db config
#define DB_NAME     "IOT_cloud"
#define DB_HOST     "localhost"
#define DB_USER     "root"
#define DB_PASSWD   "gk_1122xx"

using namespace std;  
  
class DB  
{
public:
	static DB* get_instance()
        {
            static DB instance;
            return &instance;
        }
    bool InitDB(string db_name = DB_NAME, string host = DB_HOST, string user = DB_USER, string pwd = DB_PASSWD);  
    bool Select(string sql);
    bool Query(string sql);

//private保证了直接初始化一个对象会产生编译错误
private:
	DB();
    virtual ~DB();
private:  
    MYSQL *connection;  
    MYSQL_RES *result;  
    MYSQL_ROW row;  
}; 

#endif