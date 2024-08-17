#pragma once
#include <QLineEdit>

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
    QStringList m_history;
    int m_counter = 0;
};
