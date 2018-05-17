#include "FileDownloader.h"
#include <QLoggingCategory>

FileDownloader::FileDownloader(QObject* parent) : QObject(parent) {
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::load(const QUrl& url) {
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

void FileDownloader::checkSSLSupport() {
    qDebug() << "Support SSL:  " << QSslSocket::supportsSsl()
            << "\nLib Version Number: " << QSslSocket::sslLibraryVersionNumber()
            << "\nLib Version String: " << QSslSocket::sslLibraryVersionString()
            << "\nLib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber()
            << "\nLib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
}
