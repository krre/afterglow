#include "ProcessManager.h"

class RlsManager : public ProcessManager {
    Q_OBJECT
public:
    explicit RlsManager(QObject* parent = nullptr);
    ~RlsManager();

    static RlsManager* getInstance();
    static void start();

signals:


protected slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;
};
