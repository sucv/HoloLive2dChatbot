#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Holo Chatbot");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
