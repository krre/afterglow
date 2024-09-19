#include "AutoCompleter.h"
#include "TextEditor.h"
#include "process/RlsManager.h"
#include <QtWidgets>

AutoCompleter::AutoCompleter(QObject* parent) : QCompleter(parent) {
    setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);

    m_listModel = new QStringListModel(this);
    setModel(m_listModel);

    m_tmpPath = QDir::tempPath() + "/racer.tmp";
    m_tmpFile.setFileName(m_tmpPath);

    connect(this, qOverload<const QString&>(&QCompleter::activated), this, &AutoCompleter::onActivate);
    connect(RlsManager::instance(), &RlsManager::completionResult, this, &AutoCompleter::onCompletionResult);
}

void AutoCompleter::setTextEditor(TextEditor* editor) {
    m_editor = editor;
    setWidget(editor);
}

void AutoCompleter::open() {
    if (!m_tmpFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open temporary Racer file" << m_tmpPath;
        return;
    }

    m_tmpFile.write(m_editor->document()->toPlainText().toUtf8());
    m_tmpFile.close();

    QTextCursor cursor = m_editor->textCursor();
    int row = cursor.blockNumber();
    int column = cursor.columnNumber();

    RlsManager::completion(m_tmpPath, row, column);
//    RlsManager::completion(editor->getFilePath(), row, column);
}

void AutoCompleter::onActivate(const QString& completion) {
    if (widget() != m_editor) {
        return;
    }

    QTextCursor cursor = m_editor->textCursor();
    int extra = completion.length() - completionPrefix().length();
    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(completion.right(extra));
    m_editor->setTextCursor(cursor);
}

void AutoCompleter::onCompletionResult(const QJsonArray& result) {
    m_tmpFile.remove();

    QStringList words;

    for (const QJsonValue& value : result) {
        words << value.toObject()["label"].toString();
    }

    m_listModel->setStringList(words);
    m_listModel->sort(0);

    QString prefix = m_editor->textUnderCursor();

    if (prefix != completionPrefix()) {
        setCompletionPrefix(prefix);
        popup()->setCurrentIndex(completionModel()->index(0, 0));
    }

    QRect cr = m_editor->cursorRect();
    cr.setX(cr.x() + m_editor->leftMargin());
    cr.setWidth(popup()->sizeHintForColumn(0) + popup()->verticalScrollBar()->sizeHint().width());

    complete(cr);
}
