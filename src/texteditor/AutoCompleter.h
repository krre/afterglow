#pragma once
#include <QCompleter>
#include <QFile>

class TextEditor;
class QKeyEvent;
class QStringListModel;

class AutoCompleter : public QCompleter {
public:
    explicit AutoCompleter(QObject* parent = nullptr);
    void setTextEditor(TextEditor* editor);
    void open();

private slots:
    void onActivate(const QString& completion);
    void onCompletionResult(const QJsonArray& result);

private:
    TextEditor* editor = nullptr;
    QStringListModel* listModel = nullptr;
    QFile tmpFile;
    QString tmpPath;
};
