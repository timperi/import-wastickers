#include "mainwindow.h"

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QStandardPaths>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  auto *centralWidget = new QWidget();
  setCentralWidget(centralWidget);
  auto *leiska = new QVBoxLayout();
  centralWidget->setLayout(leiska);

  leiska->setAlignment(Qt::AlignTop);

  auto browseImagesButton = new QPushButton("Import images");
  auto browsePackButton = new QPushButton("Import .wastickers");

  browseImagesButton->setSizePolicy(QSizePolicy::Policy::Preferred,
                                    QSizePolicy::Policy::Maximum);
  browsePackButton->setSizePolicy(QSizePolicy::Policy::Preferred,
                                  QSizePolicy::Policy::Maximum);

  leiska->addWidget(browseImagesButton);
  leiska->addWidget(browsePackButton);

  m_imageLayout = new FlowLayout(0, 2, 2);
  leiska->addLayout(m_imageLayout);
  connect(browseImagesButton, &QPushButton::clicked, this,
          &MainWindow::BrowseImages);
}

MainWindow::~MainWindow() {}

/*
bool MainWindow::event(QEvent *event) {
  if (event->type() == QEvent::Type::MouseButtonRelease) {
    BrowseImages();
  }
  return false;
}
*/

void MainWindow::BrowseImages() {
  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  QString baseUrl = locations.first();
  QString filter;
  auto imageList =
      QFileDialog::getOpenFileNames(this, "Avaa kuvat", baseUrl, filter);
  qDebug() << imageList;
  if (!imageList.isEmpty()) {
    populate(imageList);
  }
}

void MainWindow::populate(QList<QString> images) {
  clearLayout(m_imageLayout);

  for (auto &im : images) {
    QImage image;
    if (image.load(im)) {
      auto pixmap = QPixmap::fromImage(image).scaledToWidth(
          128, Qt::TransformationMode::SmoothTransformation);
      auto *lbl = new QLabel();
      lbl->setPixmap(pixmap);
      lbl->setFixedSize(pixmap.size());
      m_imageLayout->addWidget(lbl);
    }
  }
}

void MainWindow::clearLayout(QLayout *layout) {
  if (layout == NULL)
    return;
  QLayoutItem *item;
  while ((item = layout->takeAt(0))) {
    if (item->layout()) {
      clearLayout(item->layout());
      delete item->layout();
    }
    if (item->widget()) {
      delete item->widget();
    }
    delete item;
  }
}
