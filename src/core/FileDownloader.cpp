#include "FileDownloader.h"
#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QNetworkReply>

FileDownloader::FileDownloader(QObject* parent) : QObject(parent) {
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::load(const QUrl& url) {
    m_isBusy = true;
    QNetworkRequest request(url);
    m_reply = m_networkAccessManager.get(request);
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
    qInfo() << "Support SSL: " << QSslSocket::supportsSsl();
    qInfo() << "Lib Version Number: " << QSslSocket::sslLibraryVersionNumber();
    qInfo() << "Lib Version String: " << QSslSocket::sslLibraryVersionString();
    qInfo() << "Lib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber();
    qInfo() << "Lib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
}

void FileDownloader::abort() {
    if (m_reply) {
        m_reply->abort();
    }
}
