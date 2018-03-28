#pragma once
#include <QCompleter>

class TextEditor;

class AutoCompleter : public QCompleter {
    Q_OBJECT

public:
    explicit AutoCompleter(const QStringList& list, QObject* parent = nullptr);
    void setTextEditor(TextEditor* editor);

private slots:
    void onActivate(const QString& completion);

private:
    TextEditor* editor;
};
