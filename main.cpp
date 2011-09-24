/*! \author    Behzad Tabibian
 *  \date      Sep. 24, 2011
 */
#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
