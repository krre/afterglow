#include "ProcessManager.h"
#include <QJsonObject>
#include <QMap>

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
    static void completion(const QString& filename, int row, int column);

signals:
    void answer(const QJsonObject& message);
    void completionResult(const QJsonArray& result);

protected slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;

private:
    bool showDebug = false;
    int counter = 0;
    QMap<int, QString> identifiers;
};
