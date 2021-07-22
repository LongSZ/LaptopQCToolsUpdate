#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QProcess>
#include <QDir>

#include <iostream>
#include <fstream>

using namespace std;

#include "curl.h"
#include "Util/client_log/client_log.h"
#include "Util/md5_sha1/md5_sha1.h"

const int VERSION_PREFIX_LENGTH = 24;                                   //固定为类似字符串 "LaptopQCTools_V20210719" 的长度 + 1
const string CACHE_PATH = "cache/";                                     //下载的临时文件存放目录

class DownloadThread : public QThread
{
    Q_OBJECT
public:
    DownloadThread();
    
signals:
    void signalDownloadOK(bool ok);
    void signalDownloadProgress(int nprogress);

protected:
    void run() override;

private:
    bool download_file(CURLcode &res, string hurl, void *file, FILE *&out);
    bool is_need_update(QString local_url, QString remote_md5);
};

#endif // DOWNLOADTHREAD_H
