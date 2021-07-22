#include "downloadthread.h"

constexpr auto USERAGENT = "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36";

DownloadThread::DownloadThread()
{
    
}

/*- F --------------------------------------------------------------------------------------*\
 * Fun: 检查软件是否需要更新，算法如下：
 *      1. 配置文件 updateapp.ini 记录了所有需要经常改动的文件
 *      2. Conf目录下是本地当前的记录, cache目录下是从用品拉取的最新的记录
 *      3. 遍历cache目录下的全部记录, 如果全部跟本地一样则不需要更新, 返回false, 否则需要更新, 主要包括的情况有
 *          -- cache目录下记录与Conf目录下记录不一样.
 *          -- cache目录下记录在Conf目录下没有记录
 * 
 *      4. 具体更新方法就是把cache目录下记录的有改动的文件和Conf目录下没有记录的文件下载并复制到适当位置.
 * Arg:
 * Ret: vector<string> -- 存储了所有当前连接在电脑上的设备的id.
 *                                                                           -- By LiuHongWei
\*------------------------------------------------------------------------------------------*/
void DownloadThread::run()
{
    bool downloadOK = true;
    
    string cache_update_init = "cache/Conf/updateapp.ini";                                      //刚刚检查了更新, 确定文件一定存在
    QSettings cache_update_init_settings(cache_update_init.c_str(), QSettings::IniFormat);
    QStringList cache_all_keys = cache_update_init_settings.allKeys();
    int total_file_num = cache_all_keys.size();
    for (int i = 0; i < total_file_num; i++) {
        QString cache_key_url = cache_all_keys.at(i);                                           //key 是类似LaptopQCTools_V20210719/LaptopQCTools.exe, 去掉前缀就是本地文件路径
        cout << "cache_key_url = " << cache_key_url.toStdString() << endl;
        QString cache_value_md5 = cache_update_init_settings.value(cache_key_url).toString();   //value 是对应的 md5
        cout << "cache_value_md5 = " << cache_value_md5.toStdString() << endl;
        QString local_url = cache_key_url.mid(VERSION_PREFIX_LENGTH);                           //固定为类似字符串 "LaptopQCTools_V20210719" 的长度 + 1
        cout << "local_url = " << local_url.toStdString() << endl;
        if (is_need_update(local_url, cache_value_md5)) {
            QString remote_url = QString("http://ljtools.zhuanstatic.com/download/LaptopQCTools/") + cache_key_url;
            cout << "remote_url = " << remote_url.toStdString() << endl;
            
            //下载文件
            QString tmp_file = QString(CACHE_PATH.c_str()) + local_url;
            cout << "tmp_file = " << tmp_file.toStdString() << endl;
            QDir dir;
            dir.mkpath(tmp_file.mid(0, tmp_file.lastIndexOf('/')));
            FILE *pf = fopen(tmp_file.toStdString().c_str(), "wb");
            if (!pf) {                                                                          //无法创建文件, 下载失败
                LOG_MESSAGE((LOG_ERR, "%s %d - create file(%s) fail\n", __FILE__, __LINE__, cache_update_init.c_str()));
                downloadOK = false;
                break;
            }
            CURLcode code;
            download_file(code, remote_url.toStdString(), NULL, pf);
            if (code != CURLE_OK) {                                                             //下载文件文件失败
                LOG_MESSAGE((LOG_ERR, "%s %d - download file(%s) fail\n", __FILE__, __LINE__, remote_url.toStdString().c_str()));
                downloadOK = false;
                break;
            }
            fclose(pf);
            pf = NULL;
            
            //检验md5
            string md5 = MD5_SHA1::get_file_md5(local_url.toStdString());
            if(cache_value_md5.compare(md5.c_str(), Qt::CaseInsensitive) == 0){
                LOG_MESSAGE((LOG_INFO, "%s %d - download file(%s) success.\n", __FILE__, __LINE__, local_url.toStdString().c_str()));
            }
        }
        else {
            LOG_MESSAGE((LOG_INFO, "%s %d - no need to update file(%s).\n", __FILE__, __LINE__, local_url.toStdString().c_str()));
        }
        
        emit signalDownloadProgress((i + 1) * 100.0 / total_file_num);
    }
    
    emit signalDownloadOK(downloadOK);                                                                      //下载完成后发送信号, 通知复制所有文件到指定位置
}

bool DownloadThread::download_file(CURLcode &res, string hurl, void *file, FILE *&out)
{
    CURL* curl = curl_easy_init();
    bool ret = false;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, hurl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
        //curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &DownloadThread::callbackDownload);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, file);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);
        curl_easy_setopt(curl,CURLOPT_DNS_CACHE_TIMEOUT,0);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Cache-Control:no-cache");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        curl_easy_cleanup(curl);

        /* Check for errors */
        if (res == CURLcode::CURLE_OK) {
            ret = true;
        }
    }
    return false;
}

bool DownloadThread::is_need_update(QString remote_url, QString remote_md5)
{
    string local_update_init = "Conf/updateapp.ini";
    ifstream in_file(local_update_init);
    if (in_file.is_open()) {                                                                                //正确打开了文件
        QSettings local_update_init_settings(local_update_init.c_str(), QSettings::IniFormat);
        QStringList local_all_keys = local_update_init_settings.allKeys();
        for (int i = 0; i < local_all_keys.size(); i++) {
            QString local_key_url = local_all_keys.at(i);                                                   //key 是类似LaptopQCTools_V20210719/LaptopQCTools.exe, 去掉前缀就是本地文件路径
            QString local_url = local_key_url.mid(VERSION_PREFIX_LENGTH);                                   //固定为类似字符串 "LaptopQCTools_V20210719" 的长度 + 1
            if(local_url.compare(remote_url, Qt::CaseInsensitive) == 0){                                    //有相同的key, 则需要比较md5
                QString local_value_md5 = local_update_init_settings.value(local_key_url, "").toString();   //value 是对应的 md5
                if(local_value_md5.compare(remote_md5, Qt::CaseInsensitive) == 0){                          //md5相同则不需要更新, 否则需要更新
                    return false;
                }
            }
            else {
                continue;
            }
        }
    }
    else {                                                                  //打开文件失败，说明本地配置丢失，需要重新更新
        LOG_MESSAGE((LOG_ERR, "%s - %d : open file(%s) failed, is it exist?\n", __FILE__, __LINE__, local_update_init.c_str()));
        return true;
    }
    
    return true;
}
