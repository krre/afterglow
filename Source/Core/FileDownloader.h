#pragma once
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject {
    Q_OBJECT
public:
    explicit FileDownloader(const QUrl& url, QObject* parent = nullptr);

    QByteArray getDownloadedData() const;

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* reply);

private:
    QNetworkAccessManager networkAccessManager;
    QByteArray downloadedData;
};
