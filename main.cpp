#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;

#ifdef Q_OS_WIN
  w.resize(480, 600);
  w.show();
#else
  w.showMaximized();
#endif

  return a.exec();
}
