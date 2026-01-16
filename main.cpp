#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv); // This starts the GUI engine
    MainWindow w;
    w.show();
    return a.exec();
}
