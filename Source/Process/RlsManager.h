#include "ProcessManager.h"
#include <QJsonObject>

class RlsManager : public ProcessManager {
    Q_OBJECT
public:
    explicit RlsManager(QObject* parent = nullptr);
    ~RlsManager();

    static RlsManager* getInstance();
    static void start();
    static void initialize(const QString& projectPath);
    static void shutdown();
    static void setShowDebug(bool showDebug);
    static void send(const QString& method, const QJsonObject& params = QJsonObject());

signals:
    void answer(const QJsonObject& message);

protected slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;

private:
    bool showDebug = false;
    int counter = 0;
};
