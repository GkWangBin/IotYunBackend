/**
 * author: wangbin (gkwangbin@126.com)
 * date: 2017-05-23
 */
#include "logic.h"
#include "log.h"
#include "redis.h"
#include "DB.h"

#define  DEV_EXIST      1
#define  DEV_NOEXIST    0

using namespace std;

/**
 * [getSql description]
 * @param  format  [description]
 * @param  list [description]
 * @return      [description]
 */
int GetSql(char * sql, const char* format,...)
{
    va_list valst;
    va_start(valst, format);
    int len = vsnprintf(sql, INFO_MAX_LEN-1, format, valst);

    return len;
}

/**
 * [GetIndex description]
 * @param  devId [description]
 * @return       [description]
 */
int GetIndex(const string &devId)
{
    return 0;
}

/**
 * [SplitString description]
 * @param  str    [description]
 * @param  sensor [description]
 * @param  sep    [description]
 * @return        [description]
 */
bool SplitString(const string& str, sensor_s* sensor, const string& sep)
{
    
    string::size_type pos1, pos2;
    vector<std::string> vec;

    pos2 = str.find(sep);
    if(string::npos == pos2)
    {
        return  false;
    }

    pos1 = 0;
    while(string::npos != pos2)
    {
        vec.push_back(str.substr(pos1, pos2-pos1));
     
        pos1 = pos2 + sep.size();
        pos2 = str.find(sep, pos1);
    }
    if(pos1 != str.length())
    {
        vec.push_back(str.substr(pos1));
    }

    if(vec[0] != "" && vec[1] != "")
    {
        sensor->devId = vec[0];
        sensor->info = vec[1];
        return true;
    }
    return false;
}

/**
 * [CheckDevInCache description]
 * @param  devId [description]
 * @return       [description]
 */
bool CheckDevIdInCache(const string &devId)
{
    Redis redis;
    int result = 0;
    if(redis.ConnectRedis()){
        if (redis.SetIsMember(devId))
        {
            result = redis.GetRedisInt();
        }
    }
    //cout << "result:" << result << endl;
    if(DEV_EXIST == result)
    {
        return true;
    }

    return false;
}

/**
 * [PutDeviceIdToCache 添加缓存不做严格检查]
 * @param  devId [description]
 * @return       [description]
 */
bool PutDeviceIdToCache(const string &devId)
{
    Redis redis;
    if(redis.ConnectRedis()){
        redis.SetAdd(devId);
    }
    
    return true;
}

/*
//should be modified

bool CheckAuthen(struct sensor_s data)
{
    string str = data.devId + SECRET_KEY + data.info;
    char auth[AUTH_CHECK_LEN] = {'\0'};

    for (int i = 0; i < str.size(); ++i)
    {
        auth[i % AUTH_CHECK_LEN] ^= str[i];
    }

    if(0 == strcmp(auth, data.auth))
    {
        return true;
    }

    return false;
}
*/

/**
 * [AnalyseData description]
 * @param  recvInfo [description]
 * @param  info     [description]
 * @return          [description]
 */
int AnalyseData(const string &recvInfo, sensor_s* info)
{
    if(!SplitString(recvInfo, info, "|"))
    {
        return ERR_ILLEGALLY;
    }
    
    //check dev whether or not belong to me
    if(CheckDevIdInCache(info->devId))
    {
        //cout << "_____" << endl;
        return ERR_SUCCESS;
    }
    else
    {
        char sql[INFO_MAX_LEN] ={"\0"};
        GetSql(sql, SQL_SELECT, info->devId.c_str());
        
        if(DB::get_instance()->Select(sql))
        {
            cout << sql << endl;
            PutDeviceIdToCache(info->devId);
            return ERR_SUCCESS;
        }
        LOG_WARN("%s:%d(%s), %s:%s", __FILE__, __LINE__, __FUNCTION__, 
            "no device!", info->devId.c_str());
        return ERR_NODEVICE;
    }
    return ERR_SUCCESS;
}

/**
 * [FilterIllgerChar description]
 * @param str [description]
 */
void FilterIllgelChar(string &str)
{
    str = str.replace(str.find("\n"), 1, "\0");
}

/**
 * [ProcessData description]
 * @param  recv [description]
 * @return      [description]
 */
int ProcessData(const char * recv)
{

    string recvInfo(recv);
    FilterIllgelChar(recvInfo);
    
    struct sensor_s info = {
        .devId = "",
        .info= "",
        .ctime = (int)time(NULL),
        .flag = 1,
    };

    int err = AnalyseData(recvInfo, &info);
    //cout << "errno: " << err << endl;
    if(err)
    {
        LOG_WARN("%s:%d(%s), %s: %d", __FILE__, __LINE__, __FUNCTION__, 
            "recv info analyse error!", err);
        return err;
    }

    int tableIndex = GetIndex(info.devId);
    char sql[INFO_MAX_LEN] ={"\0"};
    GetSql(sql, SQL_INSERT, tableIndex, info.devId.c_str(), 
        info.info.c_str(), info.ctime, info.flag);
    cout << sql << __LINE__ <<endl;
    DB::get_instance()->Query(sql);

    return ERR_SUCCESS;
}
