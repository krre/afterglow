#include <QLineEdit>
#include <QVector>

class CommandLine : public QLineEdit {
    Q_OBJECT
public:
    explicit CommandLine(QWidget* parent = nullptr);
    void run();

signals:
    void focusReceived();

protected:
    void focusInEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QVector<QString> history;
    int counter = 0;
};
