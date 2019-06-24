//
// Created by quent on 21/06/2019.
//

#ifndef SERVER_IMAGEGETTER_H
#define SERVER_IMAGEGETTER_H

#include <string>
#include <QObject>
#include <QtCore>
#include <QtWidgets/QMainWindow>
#include "FileDownloader.h"
#include <iostream>

using std::string;

class ImageGetter : public QObject
{
    Q_OBJECT
private:
    QString url;
    FileDownloader* m_pImgCtrl;
    QPixmap buttonImage;
    bool downloaded = false;
public:
    ImageGetter(const QString &url) : url(url){
        QUrl imageUrl = QUrl(url);
        m_pImgCtrl = new FileDownloader(imageUrl, this);
        connect(m_pImgCtrl, SIGNAL (downloaded()), this, SLOT (loadImage()));
    };

    ~ImageGetter()
    {
        delete m_pImgCtrl;
    }

    const QPixmap &getImage() const{return buttonImage;}

    bool isDownloaded() const{return downloaded;}

private slots:
    void loadImage()
    {
        std::cout << "Downloaded" << std::endl;
        downloaded = true;
        buttonImage.loadFromData(m_pImgCtrl->downloadedData());
        emit ok();
    }

signals:
    void ok();
};


#endif //SERVER_IMAGEGETTER_H
