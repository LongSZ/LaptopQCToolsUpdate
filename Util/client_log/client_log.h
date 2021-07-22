/*- C --------------------------------------------------------------------------------------*\
    Version     Author          Desciption
    1.00        LiuHongWei      客户端日志工具, 可自由设置按日志级别输出、按指定的标签输出, 具体参考test函数.
    ...
\*------------------------------------------------------------------------------------------*/

#ifndef CLIENT_LOG_H
#define CLIENT_LOG_H

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

static const int LOG_EMERG          = 0;            //system is unusable
static const int LOG_ALERT          = 1;            //action must be taken immediately
static const int LOG_CRIT           = 2;            //critical conditions
static const int LOG_ERR            = 3;            //error conditions
static const int LOG_WARNING        = 4;            //warning conditions
static const int LOG_NOTICE         = 5;            //normal but significant condition
static const int LOG_INFO           = 6;            //informational
static const int LOG_DEBUG          = 7;            //debug-level messages
static const int LOG_UNDEFINE       = 8;            //undefine

static const char *LOG_EMERG_STR    = "EMERG: ";    //system is unusable
static const char *LOG_ALERT_STR    = "ALERT: ";    //action must be taken immediately
static const char *LOG_CRIT_STR     = "CRIT:  ";    //critical conditions
static const char *LOG_ERR_STR      = "ERR:   ";    //error conditions
static const char *LOG_WARNING_STR  = "WARN:  ";    //warning conditions
static const char *LOG_NOTICE_STR   = "NOTICE:";    //normal but significant condition
static const char *LOG_INFO_STR     = "INFO:  ";    //informational
static const char *LOG_DEBUG_STR    = "DEBUG: ";    //debug-level messages
static const char *LOG_UNDEFINE_STR = "UNDEF: ";    //undefine

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的调试标签输出调试信息.
 * Arg: token - 指定的调试标签
 *      format ... - 类似printf的参数列表
 * Ret: vector<string> -- 存储了所有当前连接在电脑上的设备的id.
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void debug_msg_out(const char *token, const char *format, ...);

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的日志级别输出调试信息，类似printf支持可变参数.
 * Arg: level - 指定的日志级别
 *      format ... - 类似printf的参数列表
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void log_msg_out(int level, const char *format, ...);

//为方便使用定义此宏，当然也可以直接调用里面的函数.
#define DEBUG_MESSAGE(x) do { debug_msg_out x;}while(0)
#define LOG_MESSAGE(x) do { log_msg_out x;}while(0)
#define PRINT_LOG_ERROR(x) LOG_MESSAGE((LOG_ERR, "%s - %d : %s\n", __FILE__, __LINE__, (x)));
#define PRINT_LOG_WARNING(x) LOG_MESSAGE((LOG_WARNING, "%s - %d : %s\n", __FILE__, __LINE__, (x)));
#define PRINT_LOG_NOTICE(x) LOG_MESSAGE((LOG_NOTICE, "%s - %d : %s\n", __FILE__, __LINE__, (x)));
#define PRINT_LOG_INFO(x) LOG_MESSAGE((LOG_INFO, "%s - %d : %s\n", __FILE__, __LINE__, (x)));
/*- F --------------------------------------------------------------------------------------*\
 * Fun: 初始化使用此日志功能的参数.
 * Arg:
 * Ret:
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void log_init();

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 测试功能
 * Arg:
 * Ret:
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void test();

#endif // CLIENT_LOG_H
