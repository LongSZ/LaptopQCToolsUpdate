#include "mainwindow.h"
#include "Util/client_log/client_log.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    log_init(); 
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
