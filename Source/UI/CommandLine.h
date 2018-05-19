#include <QLineEdit>

class CommandLine : public QLineEdit {
    Q_OBJECT
public:
    explicit CommandLine(QWidget* parent = nullptr);

signals:
    void focusReceived();

protected:
    void focusInEvent(QFocusEvent* event) override;
};
