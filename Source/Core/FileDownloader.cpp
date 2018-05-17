#include "FileDownloader.h"

FileDownloader::FileDownloader(const QUrl& url, QObject* parent) : QObject(parent) {
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);

    QNetworkRequest request(url);
    networkAccessManager.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply* reply) {
    downloadedData = reply->readAll();
    reply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::getDownloadedData() const {
    return downloadedData;
}
