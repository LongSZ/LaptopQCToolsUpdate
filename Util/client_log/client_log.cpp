#include "client_log.h"
#include <windows.h>
/*- D --------------------------------------------------------------------------------------*\
 * Fun: 实现此功能所用全局变量.
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
static int  is_debug = 1;                                       //标志是否调试
static int  debug_token_num = 0;                                //调试参数个数
static int  is_debug_all = 0;                                   //是否打印所有调试信息
static int  log_level = LOG_WARNING;                            //当前系统使用的日志级别,默认4 - LOG_WARNING
#define     MAX_LOG_LENGTH              4096                    //日志字符串最大长度
#define     MAX_DEBUG_TOKEN_NUM         1024                    //调试参数最大个数
#define     MAX_DEBUG_TOKEN_LENGTH      512                     //调试标签最大长度
#define     TMP_BUF_LENGTH              4096                    //临时缓冲区长度

#define     DEBUG_TOKEN_DELIMITER       (",")                   //调试标签分割符
#define     DEBUG_TOKEN_ALL             ("all")                 //调试所有标签时所用的标签
#define     NULL_STRING                 ("")                    //空字符串
#define     LOG_FILE_PATH               "./log/"      //日志文件的位置
char *      log_file_name = NULL;
#define     LOG_CONF_FILE_PATH          "./Conf/log.conf"            //日志配置文件的位置

char *debug_tokens[MAX_DEBUG_TOKEN_NUM];                        //存储调试参数

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 获取系统当前的日期和时间，格式如：2018-7-30 15:46:27
 * Arg:
 * Ret: char * - 指向系统当前日期和时间的指针.
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
char *get_current_date_and_time(char *date_time) {
    time_t t;
    time(&t);

    //这个是世界标准时间，即格林威治时间
    struct tm *p = gmtime(&t);
    char buf[TMP_BUF_LENGTH] = { 0 };
    //sprintf(buf, "%d-%.2d-%.2d %.2d:%.2d:%.2d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour + 8, p->tm_min, p->tm_sec);
    sprintf(buf, "%d%.2d%.2d%.2d%.2d%.2d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour + 8, p->tm_min, p->tm_sec);
    date_time = strdup(buf);

    //本地时间
    // struct tm p;
    // if (localtime_s(&p, &t)) {
    //  return NULL_STRING;
    // }
    // char buf[TMP_BUF_LENGTH] = { 0 };
    // sprintf_s(buf, "%d-%d-%d %d:%d:%d", 1900 + p.tm_year, 1 + p.tm_mon, p.tm_mday, p.tm_hour, p.tm_min, p.tm_sec);
    // date_time = _strdup(buf);
    return date_time;
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 根据指定的日志级别输出日志信息.
 * Arg: level - 指定的日志级别
 *      str   - 日志信息
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void __my_log_out(int level, const char *str) {
    char *level_str = NULL;
    switch (level) {
    case 0:
        level_str = const_cast<char *>(LOG_EMERG_STR);
        break;
    case 1:
        level_str = const_cast<char *>(LOG_ALERT_STR);
        break;
    case 2:
        level_str = const_cast<char *>(LOG_CRIT_STR);
        break;
    case 3:
        level_str = const_cast<char *>(LOG_ERR_STR);
        break;
    case 4:
        level_str = const_cast<char *>(LOG_WARNING_STR);
        break;
    case 5:
        level_str = const_cast<char *>(LOG_NOTICE_STR);
        break;
    case 6:
        level_str = const_cast<char *>(LOG_INFO_STR);
        break;
    case 7:
        level_str = const_cast<char *>(LOG_DEBUG_STR);
        break;
    default:
        level_str = const_cast<char *>(LOG_UNDEFINE_STR);
        break;
    }
    if (level <= log_level) {
        char *date_time = NULL;
#ifdef CONSOLE
        printf("%s %s %s", get_current_date_and_time(date_time), level_str, str);
#else
//        char buf[TMP_BUF_LENGTH] = { 0 };
//        sprintf(buf, "%s %s %d %s %s", get_current_date_and_time(date_time), __FILE__, __LINE__, level_str, str);
//        FILE *fp = NULL;
//        if (fopen_s(&fp, LOG_FILE_PATH, "a+") != 0) {
//            printf("open file %s failed!!!\n", LOG_FILE_PATH);
//            return;
//        }
//        fputs(buf, fp);
//        fclose(fp);
        
        ofstream out(string(LOG_FILE_PATH) + log_file_name + ".log", ios::app);
        if (!out) {
            std::cout << "open log file: " << string(LOG_FILE_PATH) + log_file_name << " failed!!!" << std::endl;
            return;
        }
        out << get_current_date_and_time(date_time) << " " << level_str << " " << str;
        out.close();
#endif
    }
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 判断日志标签有没有注册到系统，如果有注册才会输出.
 * Arg: token - 待判断的标签
 * Ret: int - 0表示没有注册到底系统，1表示注册到系统.
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
int is_debug_token_registered(const char *token) {
    if (!is_debug) {            //未开启调试
        __my_log_out(LOG_INFO, "is_debug is 0, can not debug\n");
        return 0;
    }

    if (is_debug_all) {         //打印所有调试信息
        __my_log_out(LOG_INFO, "is_debug_all is 1, output all debug info\n");
        return 1;
    }

    if (debug_token_num > 0) {
        for (int i = 0; i < debug_token_num; i++) {
            if (debug_tokens[i] != NULL_STRING && strncmp(debug_tokens[i], token, strlen(debug_tokens[i])) == 0) {
                return 1;
            }
        }

        return 0;
    }
    else {
        return 0;
    }
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 比较字符串是否相同，区分大小写.
 * Arg: s1, s2 - 待比较的字符串
 * Ret: int - 0表示相等，非0表示不相等
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
int strcasecmp(const char *s1, const char *s2) {
    size_t          ii;
    int             res = -1;

    if (!s1) {
        if (!s2) {
            return 0;
        }
        return -1;
    }
    if (!s2) {
        return 1;
    }

    for (ii = 0; (ii < 1000000) && *s1 && *s2; ii++, s1++, s2++) {
        //res = (int)(tolower(*s1) - tolower(*s2));
        res = (int)((*s1) - (*s2));
        if (res != 0) {
            break;
        }
    }

    if (ii == 1000000) {
        s1--;
        s2--;
    }

    if (!*s1) {
        if (!*s2) {
            return 0;
        }
        return -1;
    }
    if (!*s2) {
        return 1;
    }

    return (res);
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的分割符将调试标签字符串拆分成一个个标签.
 * Arg: s - 待拆分的调试标签字符串
 *      delim - 分割符，此处是","
 * Ret: 拆分出的调试标签
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
char *strtok_r(char *s, const char *delim, char **lasts) {
    const char *spanp;
    int c, sc;
    char *tok;

    if (s == NULL && (s = *lasts) == NULL) {
        return (NULL);
    }

cont:
    c = *s++;
    for (spanp = delim; (sc = *spanp++) != 0;) {
        if (c == sc) {
            goto cont;
        }
    }

    if (c == 0) {
        *lasts = NULL;
        return (NULL);
    }
    tok = s - 1;

    for (;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0) {
                    s = NULL;
                }
                else {
                    s[-1] = 0;
                }
                *lasts = s;
                return tok;
            }
        } while (sc != 0);
    }
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的分割符将调试标签字符串拆分成一个个标签并存储起来.
 * Arg: tokens - 待拆分的调试标签字符串
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void register_debug_tokens(const char *tokens) {
    char           *newp, *cp;
    char           *st = NULL;

    if (tokens == NULL || *tokens == 0) {
        return;
    }

    newp = strdup(tokens);
    cp = strtok_r(newp, DEBUG_TOKEN_DELIMITER, &st);
    while (cp) {
        if (strlen(cp) < MAX_DEBUG_TOKEN_LENGTH) {
            if (strcasecmp(cp, DEBUG_TOKEN_ALL) == 0) {
                is_debug_all = 1;
            }
            else if (debug_token_num < MAX_DEBUG_TOKEN_NUM) {
                if ('-' == *cp) {
                    ++cp;
                }
                else {
                    ;
                }
                debug_tokens[debug_token_num++] = strdup(cp);
            }
            else {
                ;
            }
        }
        else {
            ;
        }
        cp = strtok_r(NULL, DEBUG_TOKEN_DELIMITER, &st);
    }
    free(newp);
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 由debug_msg_out调用，根据指定的日志级别输出日志信息，用于支持可变参数.
 * Arg: level - 指定的日志级别
 *      str   - 日志信息
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
int __debug_log_out(int level, const char *format, va_list ap) {
#ifndef MAXISYS_ForPC
    char            buffer[MAX_LOG_LENGTH];
    int             length;
    char           *dynamic;
    va_list         aq;

    va_copy(aq, ap);
    length = vsnprintf(buffer, MAX_LOG_LENGTH, format, ap);
    va_end(ap);

    if (length == 0) {   // Empty string
        return 0;
    }

    if (length < MAX_LOG_LENGTH) {
        __my_log_out(level, buffer);
        return 0;
    }

    dynamic = (char *)malloc(length + 1);
    if (dynamic == NULL) {                      //内存分配失败,是很严重的错误
        __my_log_out(LOG_ERR, "malloc failed!!!");
        return -1;
    }
    vsnprintf(dynamic, length + 1, format, aq);
    __my_log_out(level, dynamic);
    free(dynamic);
    va_end(aq);
#endif
    return 0;
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的调试标签输出调试信息.
 * Arg: token - 指定的调试标签
 *      format ... - 类似printf的参数列表
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void debug_msg_out(const char *token, const char *format, ...) {
    if (is_debug_token_registered(token) == 1) {
        va_list         debugargs;
        va_start(debugargs, format);
        char buf[MAX_LOG_LENGTH];
        sprintf(buf, "%s %s", token, format);
        __debug_log_out(LOG_DEBUG, buf, debugargs);
        va_end(debugargs);
    }
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 按指定的日志级别输出调试信息，类似printf支持可变参数.
 * Arg: level - 指定的日志级别
 *      format ... - 类似printf的参数列表
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void log_msg_out(int level, const char *format, ...) {
    va_list         debugargs;
    va_start(debugargs, format);
    __debug_log_out(level, format, debugargs);
    va_end(debugargs);
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 测试功能.
 * Arg:
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void test() {
    DEBUG_MESSAGE(("AAA", "test: %d\n", 99));
    DEBUG_MESSAGE(("CCC", "test: %d\n", 99));
    DEBUG_MESSAGE(("BBB", "test: %s\n", "this is a test message by liuhongwei"));
    log_msg_out(7, "777\n");
    log_msg_out(6, "666\n");
    log_msg_out(5, "555\n");
    log_msg_out(4, "444\n");
    log_msg_out(3, "333\n");
    log_msg_out(2, "222\n");
    log_msg_out(1, "111\n");
    LOG_MESSAGE((LOG_EMERG, "test: %d %s\n", 0, "this is a test string"));
    LOG_MESSAGE((LOG_ALERT, "test: %d\n", 1));
    LOG_MESSAGE((LOG_CRIT, "test: %d\n", 2));
    LOG_MESSAGE((LOG_ERR, "test: %d\n", 3));
    LOG_MESSAGE((LOG_WARNING, "test: %d\n", 4));
    LOG_MESSAGE((LOG_NOTICE, "test: %d\n", 5));
    LOG_MESSAGE((LOG_INFO, "test: %d\n", 6));
    LOG_MESSAGE((LOG_DEBUG, "test: %d\n", 7));
    LOG_MESSAGE((LOG_UNDEFINE, "test: %d\n", 8));
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 初始化使用此日志功能的参数.
 * Arg:
 * Ret: 无
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void log_init() {
    /*
     文件格式: 1 0 4 token1, token2, token3
     参数依次是标志是否调试、是否打印所有调试信息、日志级别，
     接着是需要的调试标记，注意如果后面还有标记才需要加逗号
    */
    WCHAR exeFullPath[256]; // Full path
    std::string strPath = "";

    GetModuleFileName(NULL, exeFullPath, 256); //获取带有可执行文件名路径
    {
        int ansiiLen = WideCharToMultiByte(CP_ACP, 0, exeFullPath, -1, nullptr, 0, nullptr, nullptr);
        char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
        WideCharToMultiByte(CP_ACP, 0, exeFullPath, -1, pAssii, ansiiLen, nullptr, nullptr);
        strPath = std::string(pAssii);
        free(pAssii);
    }
    
    int pos = strPath.find_last_of('\\', strPath.length());
    std::string workDir = strPath.substr(0, pos);  // 返回不带有可执行文件名的路径
    std::string log_config_path = workDir + "\\Conf\\log.conf";
    ifstream in(log_config_path.c_str());//使用相对路径可能会失败
    if (!in) {
        ofstream out(LOG_FILE_PATH);
        out << "open log configure file: log.conf fail!!!" << endl;
        out.close();
        return;
    }

    in >> is_debug;                     //标志是否调试
    in >> is_debug_all;                 //是否打印所有调试信息
    in >> log_level;                    //日志级别

    string token_string = "";
    string s;
    while (in >> s) {                   //
        token_string += s;
    }
    in.close();

    register_debug_tokens(token_string.c_str());

    log_file_name = get_current_date_and_time(log_file_name);
    //test();                             //test
}




