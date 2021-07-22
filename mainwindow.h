#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "downloadthread.h"
#include "Util/md5_sha1/md5_sha1.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    bool copy_cache();
    
private slots:
    void updateProgressBar(int nprogress);
    void downloadFinished(bool bok);
    
private:
    Ui::MainWindow *ui;
    
    bool copyDir(const QString &src, const QString &dst, bool cover);
    
    DownloadThread *m_pDownloadThread;
};
#endif // MAINWINDOW_H
