#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "flowlayout.h"
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  // protected:
  //   bool event(QEvent *event) override;

private:
  FlowLayout *m_imageLayout;

private:
  void BrowseImages();
  void populate(QList<QString> images);

  //  QList<QString> m_importImages;
  QList<QLabel *> m_labels;
  void clearLayout(QLayout *layout);
  void send();
  void ImportWaStickers();

  QMap<QString, QByteArray> rawImages;
};
#endif // MAINWINDOW_H
