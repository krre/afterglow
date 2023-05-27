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
    TextEditor* m_editor = nullptr;
    QStringListModel* m_listModel = nullptr;
    QFile m_tmpFile;
    QString m_tmpPath;
};
