#include "FileDownloader.h"
#include <QLoggingCategory>

FileDownloader::FileDownloader(QObject* parent) : QObject(parent) {
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::load(const QUrl& url) {
    busy = true;
    QNetworkRequest request(url);
    reply = networkAccessManager.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply* reply) {
    downloadedData = reply->readAll();
    reply->deleteLater();
    this->reply = nullptr;
    busy = false;
    emit downloaded();
}

QByteArray FileDownloader::getDownloadedData() const {
    return downloadedData;
}

void FileDownloader::checkSSLSupport() {
    qDebug() << "Support SSL: " << QSslSocket::supportsSsl()
            << "\nLib Version Number: " << QSslSocket::sslLibraryVersionNumber()
            << "\nLib Version String: " << QSslSocket::sslLibraryVersionString()
            << "\nLib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber()
            << "\nLib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
}

void FileDownloader::abort() {
    if (reply) {
        reply->abort();
    }
}
