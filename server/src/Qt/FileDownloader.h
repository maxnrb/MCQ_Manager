//
// Created by quent on 21/06/2019.
//

#ifndef SERVER_FILEDOWNLOADER_H
#define SERVER_FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
Q_OBJECT

public:
    FileDownloader(QUrl imageUrl, QObject *parent = nullptr): QObject(parent)
    {
        connect(
                &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
                this, SLOT (fileDownloaded(QNetworkReply*))
        );

        QNetworkRequest request(imageUrl);
        m_WebCtrl.get(request);
    }

    ~FileDownloader() {

    } ;

    QByteArray downloadedData() const {
        return m_DownloadedData;
    }

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* pReply) {
        m_DownloadedData = pReply->readAll();
        //emit a signal
        pReply->deleteLater();
        emit downloaded();
    }

private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
};


#endif //SERVER_FILEDOWNLOADER_H
