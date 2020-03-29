#include "FileDownloader.h"
#include <QLoggingCategory>

FileDownloader::FileDownloader(QObject* parent) : QObject(parent) {
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::load(const QUrl& url) {
    m_isBusy = true;
    QNetworkRequest request(url);
    m_reply = networkAccessManager.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply* reply) {
    m_data = reply->readAll();
    reply->deleteLater();
    m_reply = nullptr;
    m_isBusy = false;
    emit downloaded();
}

QByteArray FileDownloader::data() const {
    return m_data;
}

void FileDownloader::checkSSLSupport() {
    qDebug() << "Support SSL: " << QSslSocket::supportsSsl()
            << "\nLib Version Number: " << QSslSocket::sslLibraryVersionNumber()
            << "\nLib Version String: " << QSslSocket::sslLibraryVersionString()
            << "\nLib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber()
            << "\nLib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
}

void FileDownloader::abort() {
    if (m_reply) {
        m_reply->abort();
    }
}
