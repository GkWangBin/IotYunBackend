/**
 * anthor: wangbin(gkwangbin@126.com)
 * date: 2017-05-17
 * desc: 数据库类
 */

#include <iostream>  
#include <cstdlib> 
#include <mysql/mysql.h> 
#include <string>
#include "DB.h"  
#include "log.h"

using namespace std;  

DB::DB()  
{
    connection = mysql_init(NULL); // 初始化数据库连接变量  
    if(connection == NULL)  
    {  
        LOG_ERROR("%s", "init DB error");
        exit(1);  
    }  
}  
  
DB::~DB()  
{  
    if(connection != NULL)  // 关闭数据库连接  
    {  
        mysql_close(connection);  
    }  
}

/**
 * [MyDB::initDB description]
 * @param  host    [description]
 * @param  user    [description]
 * @param  pwd     [description]
 * @param  db_name [description]
 * @return         [description]
 */
bool DB::InitDB(string db_name, string host, string user, string pwd)  
{  
    int times = 0;
    while (times < 2) 
    {
        connection = mysql_real_connect(connection, host.c_str(),  
            user.c_str(), pwd.c_str(), db_name.c_str(), 0, NULL, 0);
        if (connection != NULL)
        {
            LOG_INFO("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "connect db secccess!");
            break;
        }
    }
    if(connection == NULL)  
    {
        LOG_ERROR("%s,%s", "connect DB error: ", db_name.c_str());
        exit(1);  
    }

    return true;  
}

/**
 * [MyDB::exeSQL description]
 * @param  sql [description]
 * @return     [description]
 */
bool DB::Select(string sql)  
{  
    if((sql.find("select") < 0) && (sql.find("SELECT") < 0)){
        LOG_ERROR("%s,%s", "query grammar error: ", sql.c_str());
        return false;
    }
    if(mysql_query(connection, sql.c_str()))  
    {  
        Log::get_instance()->write_log(3, "%s, %d, %s, %s, ",__FILE__, __LINE__, mysql_error(connection),sql.c_str());
        //exit(1);
        return false;
    }  
    else
    {
        //format select result
        //LOG_INFO("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "query secccess!");
        result = mysql_use_result(connection);  
        for(int i=0; i < mysql_field_count(connection); ++i)  
        {  
            row = mysql_fetch_row(result);  
            if(row <= 0)  
            {
                return false;
            }
            if (mysql_num_fields(result) > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
            /*
            for(int j=0; j < mysql_num_fields(result); ++j)  
            {  
                cout << row[j] << " ";
            }
            cout << endl;
            */
        }
        mysql_free_result(result);
    }
    return false;  
}

bool DB::Query(string sql)
{
    if((sql.find("insert") < 0) && (sql.find("INSERT") < 0)){
        LOG_WARN("%s,%s", "query grammar error: ", sql.c_str());
        return false;
    }
    if(mysql_query(connection, sql.c_str()))  
    {  
        Log::get_instance()->write_log(3, "%s:%d, %s, sql: %s", __FILE__, __LINE__,mysql_error(connection), sql.c_str());
        return false;
    }
    return true;
}
