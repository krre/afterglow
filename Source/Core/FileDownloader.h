#pragma once
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject {
    Q_OBJECT
public:
    explicit FileDownloader(QObject* parent = nullptr);

    void load(const QUrl& url);

    QByteArray getDownloadedData() const;
    void checkSSLSupport();
    bool isBusy() const { return busy; }

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* reply);

private:
    QNetworkAccessManager networkAccessManager;
    QByteArray downloadedData;
    bool busy = false;
};
