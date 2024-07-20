#include "mainwindow.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QStandardPaths>
#include <QWidget>

#include <unzip.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  auto *centralWidget = new QWidget();
  setCentralWidget(centralWidget);
  auto *leiska = new QVBoxLayout();
  centralWidget->setLayout(leiska);

  leiska->setAlignment(Qt::AlignTop);

  auto browseImagesButton = new QPushButton("Import images");
  auto browsePackButton = new QPushButton("Import .wastickers");
  auto sendToWhatsApp = new QPushButton("Send stickers to WhatsApp");

  browseImagesButton->setSizePolicy(QSizePolicy::Policy::Preferred,
                                    QSizePolicy::Policy::Maximum);
  browsePackButton->setSizePolicy(QSizePolicy::Policy::Preferred,
                                  QSizePolicy::Policy::Maximum);
  sendToWhatsApp->setSizePolicy(QSizePolicy::Policy::Preferred,
                                QSizePolicy::Policy::Maximum);

  leiska->addWidget(browseImagesButton);
  leiska->addWidget(browsePackButton);
  leiska->addWidget(sendToWhatsApp);

  m_imageLayout = new FlowLayout(0, 2, 2);
  leiska->addLayout(m_imageLayout);

  connect(browseImagesButton, &QPushButton::clicked, this,
          &MainWindow::BrowseImages);
  connect(browsePackButton, &QPushButton::clicked, this,
          &MainWindow::ImportWaStickers);
  connect(sendToWhatsApp, &QPushButton::clicked, this, &MainWindow::send);
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

void MainWindow::ImportWaStickers() {

  QString zlibVer = zlibVersion();
  qDebug() << "zlib version:" << zlibVer;

  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  QString baseUrl = locations.first();
  QString filter;
  auto imageList =
      QFileDialog::getOpenFileNames(this, "Select images", baseUrl, filter);

  auto info = imageList.join("\n");
  QMessageBox::information(this, "You selected:", info);

  if (!imageList.isEmpty()) {
    auto pack = imageList.first();
    QMessageBox::information(this, "Info",
                             QString("Trying to open %1").arg(pack));
    auto zFile = unzOpen64(pack.toUtf8().data());
    if (zFile) {
      unz_global_info globalInfo;
      if (unzGetGlobalInfo(zFile, &globalInfo) == UNZ_OK) {
        auto numEntries = globalInfo.number_entry;
        QMessageBox::information(
            this, "Info",
            QString("Number of entries in global info: %1").arg(numEntries));

        clearLayout(m_imageLayout);

        for (size_t i = 0; i < numEntries; ++i) {
          unz_file_info64 fileInfo;
          char filename[1024];
          char extrafield[1024];
          char comment[1024];
          if (unzGetCurrentFileInfo64(zFile, &fileInfo, filename, 1024,
                                      extrafield, 1024, comment,
                                      1024) == UNZ_OK) {

            QString fname = filename;
            // Check if this entry is a directory or file.
            if (fname.endsWith('/')) {
              // Entry is a directory, so skip it
            } else if (fname.endsWith(".webp")) {
              quint32 uncompressedSize = fileInfo.uncompressed_size;
              qDebug() << "Found .webp file:" << fname
                       << "compressed size:" << fileInfo.compressed_size
                       << "uncompressed size:" << uncompressedSize;
              if (unzOpenCurrentFile(zFile) == UNZ_OK) {
                QByteArray readBuffer(uncompressedSize, 0);
                if (unzReadCurrentFile(zFile, readBuffer.data(),
                                       uncompressedSize) > 0) {
                  // ok!
                  QImage image;
                  if (image.loadFromData(readBuffer)) {
                    auto pixmap = QPixmap::fromImage(image).scaledToWidth(
                        128, Qt::TransformationMode::SmoothTransformation);
                    auto *lbl = new QLabel();
                    lbl->setPixmap(pixmap);
                    lbl->setFixedSize(pixmap.size());
                    m_imageLayout->addWidget(lbl);

                    QFileInfo info(fname);
                    auto name = info.baseName();
                    rawImages[name] = readBuffer;
                  } else {
                    QMessageBox::information(
                        this, "Info",
                        QString("cannot load image from data: %1").arg(fname));
                  }
                } else {
                  QMessageBox::information(
                      this, "Info",
                      QString("unzReadCurrentFile failed: %1").arg(fname));
                }
              } else {
                QMessageBox::information(
                    this, "Info", QString("Cannot open file: %1").arg(fname));
              }
            } else {
              // QMessageBox::information(this, "Info", QString("file is not
              // .webp: %1").arg(fname));
            }

            if (unzGoToNextFile(zFile) != UNZ_OK) {
              QMessageBox::information(
                  this, "Error",
                  QString("cannot go to next file: %1").arg(fname));
              break;
            }
          } else {
            QMessageBox::information(
                this, "Error",
                QString("cannot unzGetCurrentFileInfo64: %1").arg(pack));
            break;
          }
        }
      } else {
        QMessageBox::information(this, "Error",
                                 QString("unzGetGlobalInfo %1").arg(pack));
      }

      unzClose(zFile);
    } else {
      QMessageBox::information(this, "Error",
                               QString("Cannot open %1").arg(pack));
    }
  } else {
    QMessageBox::information(this, "Error", "Nothing selected");
  }
}

void MainWindow::BrowseImages() {
  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  QString baseUrl = locations.first();
  QString filter;
  auto imageList =
      QFileDialog::getOpenFileNames(this, "Select images", baseUrl, filter);
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

      QFileInfo info(im);
      auto name = info.baseName();
      QFile original(im);
      if (original.open(QFile::OpenModeFlag::ReadOnly)) {
        rawImages[name] = original.readAll();
        original.close();
      }
    }
  }
}

void MainWindow::clearLayout(QLayout *layout) {
  rawImages.clear();
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

void MainWindow::send() {
  // tray = (Base64 representation of the PNG, not WebP, data of the tray image)
  // image_data = (Base64 representation of the WebP, not PNG, data of the
  // sticker image) emojis = (Array of emoji strings.Maximum of 3 emoji)

  QString sticker = R"({
    "image_data" : "@IMAGE_DATA@",
    "emojis" : [ "@EMOJI_CATEGORY@" ]
  })";

  QString json = R"({
  "ios_app_store_link" : "@APP_STORE_LINK@",
  "android_play_store_link" : "@PLAY_STORE_LINK@",
  "identifier" : "@IDENTIFIER@",
  "name" : "@NAME@",
  "publisher" : "@PUBLISHER@",
  "tray_image" : "@TRAY_IMAGE@",
  "stickers" : [@STICKERS@]
})";

  QMap<QString, QString> params;
  params["APP_STORE_LINK"] = "";
  params["PLAY_STORE_LINK"] = "";
  params["IDENTIFIER"] = "";
  params["NAME"] = "";
  params["PUBLISHER"] = "";
  params["TRAY_IMAGE"] = "";
  QStringList imageList;
  for (const auto &keyName : rawImages.keys()) {
    auto data = rawImages.value(keyName);
    auto data64 = data.toBase64();
    QString s = sticker;
    s.replace("@IMAGE_DATA@", data64);
    s.replace("@EMOJI_CATEGORY@", ""); //  ?
    imageList.append(s);
  }
  params["STICKERS"] = imageList.join(',');

  for (const auto &pName : params.keys()) {
    json.replace("@" + pName + "@", params.value(pName));
  }

  // qDebug().noquote() << json;

  QClipboard *cb = QGuiApplication::clipboard();
  auto md = QMimeData();
  md.setData("application/json", json.toUtf8());
  cb->setMimeData(&md);

  QDesktopServices::openUrl(QUrl("whatsapp://stickerPack"));
}
