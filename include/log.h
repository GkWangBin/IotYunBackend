/**
 * author:wangbin(gkwangbin@126.com)
 * date: 2017-05-15
 * desc: log model
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include <stdarg.h>
#include <pthread.h>

//log config
#define LOG_PATH    "./log/mylog.log"
#define BUF_SIZE    512
#define LOG_COUNT   2000000

using namespace std;

class Log
{
    public:
        /**
         * [get_instance description]
         * @return [description]
         * desc:单例模式中的懒汉模式，实例在第一次使用时被初始化
         */
        static Log* get_instance()
        {
            static Log instance;
            return &instance;
        }

        bool init(const char* file_name = LOG_PATH, int log_buf_size = BUF_SIZE, int split_lines = LOG_COUNT);

        void write_log(int level, const char* format, ...);

        void flush(void);
    private:
        Log();
        virtual ~Log();

    private:
        
        pthread_mutex_t *m_mutex;
        char dir_name[128];
        char log_name[128];
        int m_split_lines;
        int m_log_buf_size;
        long long  m_count;
        int m_today;
        FILE *m_fp;
        char *m_buf;
};

#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, __VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, format, __VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, format, __VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3, format, __VA_ARGS__)

#endif