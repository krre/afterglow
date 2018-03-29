#pragma once
#include <QCompleter>

class TextEditor;
class QKeyEvent;
class QStringListModel;

class AutoCompleter : public QCompleter {
    Q_OBJECT

public:
    explicit AutoCompleter(QObject* parent = nullptr);
    void setTextEditor(TextEditor* editor);
    void open(QKeyEvent* event);

private slots:
    void onActivate(const QString& completion);

private:
    TextEditor* editor;
    QStringListModel* listModel;
};
