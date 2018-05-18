#include "ProcessManager.h"

class RlsManager : public ProcessManager {
    Q_OBJECT
public:
    explicit RlsManager(QObject* parent = nullptr);
    ~RlsManager();

    static RlsManager* getInstance();
    static void start();
    static void setShowDebug(bool showDebug);

signals:

protected slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;

private:
    bool showDebug = false;
};
