#pragma once
#include <QNetworkAccessManager>

class FileDownloader : public QObject {
    Q_OBJECT
public:
    explicit FileDownloader(QObject* parent = nullptr);

    void load(const QUrl& url);

    QByteArray data() const;
    void checkSSLSupport();
    void abort();
    bool isBusy() const { return m_isBusy; }

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* m_reply);

private:
    QNetworkAccessManager m_networkAccessManager;
    QByteArray m_data;
    bool m_isBusy = false;
    QNetworkReply* m_reply = nullptr;
};
