#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    m_pDownloadThread = new DownloadThread;
    connect(m_pDownloadThread, SIGNAL(signalDownloadProgress(int)), this, SLOT(updateProgressBar(int)), Qt::QueuedConnection);
    connect(m_pDownloadThread, SIGNAL(signalDownloadOK(bool)), this, SLOT(downloadFinished(bool)), Qt::QueuedConnection);
    m_pDownloadThread->start();
}

MainWindow::~MainWindow()
{
    if(m_pDownloadThread){
        m_pDownloadThread->terminate();
        m_pDownloadThread->wait();
        delete m_pDownloadThread;
        m_pDownloadThread = NULL;
    }
    
    delete ui;
}

bool MainWindow::copy_cache()
{
    return copyDir(CACHE_PATH.c_str(), "./", true);
}

void MainWindow::updateProgressBar(int nprogress)
{
    ui->progressBar->setValue(nprogress);
}

void MainWindow::downloadFinished(bool bok)
{
    //复制所有下载到cache目录下的文件到适当目录
    if (bok) {
        bok = copy_cache();
    }
    
    //更新完成, 是否立即体验
    if(bok){
        if(QMessageBox::Yes == QMessageBox::question(this, "Info", QStringLiteral("更新完成，是否立即体验!"), QMessageBox::Yes|QMessageBox::No)){
             QProcess process(0);
             process.setProgram("LaptopQCTools.exe");
             process.startDetached();
             close();
        }
    }
}

bool MainWindow::copyDir(const QString &src, const QString &dst, bool cover)
{
    QDir from(src);  
    QDir to(dst); 
    
    if(!to.exists()){    // 如果目标目录不存在, 则进行创建 
        if(!to.mkdir(to.absolutePath())) {
            return false;
        }
    }
    
    QFileInfoList fileInfoList = from.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
            continue;
        }
        //递归复制目录
        if (fileInfo.isDir()){
            QString tmp_from = fileInfo.filePath();
            QString tmp_to = to.filePath(fileInfo.fileName());
            cout << "tmp_from = " << tmp_from.toStdString() << ", tmp_to = " << tmp_to.toStdString() << endl;
            if (!copyDir(fileInfo.filePath(), to.filePath(fileInfo.fileName()), cover)) {
                return false;
            }
        }
        //复制文件
        else{
            if (cover && to.exists(fileInfo.fileName())){
                to.remove(fileInfo.fileName());
            }
            if (!QFile::copy(fileInfo.filePath(), to.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }
    return true;
}
