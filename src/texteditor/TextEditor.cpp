#include "TextEditor.h"
#include "SyntaxHighlightManager.h"
#include "LineNumberArea.h"
#include "Highlighter.h"
#include "AutoCompleter.h"
#include "core/Settings.h"
#include "core/Constants.h"
#include <QAbstractItemView>
#include <QFileInfo>
#include <QPainter>

TextEditor::TextEditor(QString filePath, SyntaxHighlightManager* syntaxHighlightManager, QWidget* parent) :
    QPlainTextEdit(parent), m_syntaxHighlightManager(syntaxHighlightManager), m_filePath(filePath) {
    setFrameShape(QFrame::NoFrame);

    const QString& family = Settings::value("editor.font.family").toString();
    int size = Settings::value("editor.font.size").toInt();
    QFont font(family, size);
    document()->setDefaultFont(font);

    setWordWrapMode(QTextOption::NoWrap);

    m_lineNumberArea = new LineNumberArea(this);

    QFileInfo fi(filePath);

    if (m_syntaxHighlightManager->hasExtension(fi.suffix())) {
        m_highlighter = new Highlighter(fi.suffix(), document(), m_syntaxHighlightManager);
    }

    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
    connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    connect(this, &TextEditor::textChanged, [this] {
        emit documentModified(this);
    });

    readFile();
}

void TextEditor::setFilePath(const QString& filePath) {
    m_filePath = filePath;
}

void TextEditor::setAutoCompleter(AutoCompleter* completer) {
    m_completer = completer;

    if (completer) {
        completer->setTextEditor(this);
    }
}

void TextEditor::saveFile() {
    if (!document()->isModified()) return;

    if (Settings::value("editor.cleanTrailingWhitespaceOnSave").toBool()) {
       cleanTrailingWhitespace();
    }

    QFile file(m_filePath);

    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toPlainText();
        document()->setModified(false);
        emit documentModified(this);
    } else {
        qWarning() << "Failed to open file for writing" << m_filePath;
    }
}

QString TextEditor::modifiedName() const {
    QFileInfo fi(m_filePath);
    return fi.fileName() + (document()->isModified() ? "*" : "");
}

QPoint TextEditor::cursorPosition() {
    QTextCursor cursor = textCursor();
    return QPoint(cursor.positionInBlock(), cursor.blockNumber());
}

void TextEditor::setCursorPosition(const QPoint& pos) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, pos.y());
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos.x());
    setTextCursor(cursor);
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(Const::Color::LineNumberArea));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(Const::Color::LineNumber));
            painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int TextEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());

    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    digits = qMax(Settings::value("editor.numberAreaDigits").toInt(), digits);
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void TextEditor::toggleSingleLineComment() {
    QTextCursor cursor = textCursor();
    int startRow = cursor.blockNumber();
    int endRow = startRow;

    bool selectionFromBeginOfBlock = false;
    bool addCommentMode = false;

    if (cursor.hasSelection()) {
        startRow = document()->findBlock(cursor.selectionStart()).blockNumber();
        endRow = document()->findBlock(cursor.selectionEnd()).blockNumber();

        if (cursor.selectionStart() == cursor.block().position()) {
            selectionFromBeginOfBlock = true;
        }
    }

    QVector<int> commentPositions(endRow - startRow + 1, -1);

    // Find commented lines and set add or remove comment mode.
    for (int row = startRow, i = 0; row <= endRow; row++, ++i) {
        QTextBlock block = document()->findBlockByLineNumber(row);
        if (!block.text().size())  continue;

        int pos = 0;

        while (block.text().at(pos) == ' ') {
            pos++;
        };

        if (block.text().at(pos) == '/' && block.text().at(pos + 1) == '/') {
            commentPositions[i] = pos;
        } else {
            addCommentMode = true;
        }
    }

    cursor.beginEditBlock();

    for (int row = startRow, i = 0; row <= endRow; row++, ++i) {
        QTextBlock block = document()->findBlockByLineNumber(row);
        if (!block.text().size()) continue;

        if (addCommentMode) {
            cursor.setPosition(block.position());
            cursor.insertText("//");
        } else {
            if (commentPositions[i] != -1) {
                cursor.setPosition(block.position() + commentPositions[i]);
                cursor.deleteChar();
                cursor.deleteChar();
            }
        }
    }

    cursor.endEditBlock();

    // Add begin of comment chars to selection
    if (addCommentMode && selectionFromBeginOfBlock) {
        extendSelectionToBeginOfComment();
    }
}

void TextEditor::toggleBlockComment() {
    QTextCursor cursor = textCursor();

    if (!cursor.hasSelection()) return;

    bool alreadyCommented = false;

    int size = cursor.selectedText().size();

    if (size >= 4 && cursor.selectedText().left(2) == "/*" &&
            cursor.selectedText().right(2) == "*/") {
        alreadyCommented = true;
    }

    cursor.beginEditBlock();

    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    if (alreadyCommented) {
        // Remove comment chars in reverse order
        cursor.setPosition(end);
        cursor.deletePreviousChar();
        cursor.deletePreviousChar();

        cursor.setPosition(start);
        cursor.deleteChar();
        cursor.deleteChar();
    } else {
        // Insert comment chars
        cursor.setPosition(start);
        cursor.insertText("/*");
        cursor.setPosition(end + 2); // End of selection plus inserted above two chars /*
        cursor.insertText("*/");
    }

    cursor.endEditBlock();

    if (!alreadyCommented) {
        extendSelectionToBeginOfComment();
    }
}

void TextEditor::openAutoCompleter() {
    m_completer->open();
}

void TextEditor::joinLines() {
    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    QTextBlock nextBlock = block.next();

    if (!nextBlock.isValid()) return;

    cursor.beginEditBlock();

    cursor.movePosition(QTextCursor::NextBlock);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString cutLine = cursor.selectedText();

    // Collapse leading whitespaces to one or insert whitespace
    cutLine.replace(QRegularExpression(QLatin1String("^\\s*")), QLatin1String(" "));
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    cursor.movePosition(QTextCursor::PreviousBlock);
    cursor.movePosition(QTextCursor::EndOfBlock);

    cursor.insertText(cutLine);

    cursor.endEditBlock();
}

void TextEditor::duplicateLine() {
    QTextCursor cursor = textCursor();

    cursor.beginEditBlock();

    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();
    cursor.movePosition(QTextCursor::NextBlock);
    cursor.insertText(line + '\n');

    cursor = textCursor();
    cursor.movePosition(QTextCursor::Down);
    setTextCursor(cursor);

    cursor.endEditBlock();
}

void TextEditor::cutLine() {
    QTextCursor cursor = textCursor();

    cursor.beginEditBlock();

    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    setTextCursor(cursor);
    cut();

    cursor.endEditBlock();
}

void TextEditor::autoindent() {
    QTextCursor cursor = textCursor();
    int row = cursor.blockNumber();

    if (row > 0) {
        QTextBlock block = document()->findBlockByLineNumber(row - 1);
        int count = 0;

        for (; count < block.text().size(); count++) {
            if (block.text().at(count) != ' ') {
                break;
            }
        }

        if (count > 0) {
            insertPlainText(QString(count, ' '));
        }
    }
}

void TextEditor::extendSelectionToBeginOfComment() {
    QTextCursor cursor = textCursor();
    const int countOfCommentChars = 2; // or /*
    int startCommentChars = cursor.selectionStart() - countOfCommentChars;
    cursor.setPosition(cursor.selectionEnd());
    cursor.setPosition(startCommentChars, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

void TextEditor::increaseIndent() {
    QTextCursor cursor = textCursor();
    int startRow = cursor.blockNumber();
    int endRow = startRow;

    if (cursor.hasSelection()) {
        startRow = document()->findBlock(cursor.selectionStart()).blockNumber();
        endRow = document()->findBlock(cursor.selectionEnd()).blockNumber();
    }

    cursor.beginEditBlock();

    int indent = Settings::value("editor.indent").toInt();

    for (int row = startRow; row <= endRow; row++) {
        QTextBlock block = document()->findBlockByLineNumber(row);

        if (block.blockNumber() == blockCount() - 1 && !block.text().size()) {
            // Last line
            break;
        }

        cursor.setPosition(block.position());

        int charPos = cursor.position() - block.position();
        int addSpaces = charPos % indent;

        if (addSpaces) {
            addSpaces = indent - addSpaces;
        } else {
            addSpaces = indent;
        }

        cursor.insertText(QString(addSpaces, ' '));
    }

    cursor.endEditBlock();
}

void TextEditor::decreaseIndent() {
    QTextCursor cursor = textCursor();
    int startRow = cursor.blockNumber();
    int endRow = startRow;

    if (cursor.hasSelection()) {
        startRow = document()->findBlock(cursor.selectionStart()).blockNumber();
        endRow = document()->findBlock(cursor.selectionEnd()).blockNumber();
    }

    cursor.beginEditBlock();

    int indent = Settings::value("editor.indent").toInt();

    for (int row = startRow; row <= endRow; row++) {
        QTextBlock block = document()->findBlockByLineNumber(row);
        if (!block.text().size()) continue;

        cursor.setPosition(block.position());

        int count = 0;

        for (; count < block.text().size(); count++) {
            if (block.text().at(count) != ' ') {
                break;
            }
        }

        int removeSpaces = count % indent;

        if (!removeSpaces && count) {
            removeSpaces = indent;
        }

        while (removeSpaces) {
            cursor.deleteChar();
            removeSpaces--;
        }
    }

    cursor.endEditBlock();
}

void TextEditor::goToLine(int line) {
    int row =  qMin(qMax(0, line - 1), blockCount() - 1);
    QTextBlock block = document()->findBlockByLineNumber(row);
    QTextCursor cursor = textCursor();
    cursor.setPosition(block.position());
    setTextCursor(cursor);
}

void TextEditor::cleanTrailingWhitespace() {
    QTextCursor cursor = textCursor();

    for (int i = 0; i < blockCount(); ++i) {
        QTextBlock block = document()->findBlockByNumber(i);
        int size = block.text().size();
        if (!size) continue;

        int pos = size - 1;

        while (pos >= 0 && block.text().at(pos) == ' ') {
            cursor.setPosition(block.position() + pos);
            cursor.deleteChar();
            pos--;
        }
    }
}

void TextEditor::keyPressEvent(QKeyEvent* event) {
    if (m_completer && m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (event->key()) {
           case Qt::Key_Enter:
           case Qt::Key_Return:
           case Qt::Key_Escape:
           case Qt::Key_Tab:
           case Qt::Key_Backtab:
                event->ignore();
                return; // let the completer do default behavior
           default:
               break;
        }
    }

    if (event->key() == Qt::Key_Tab) {
        increaseIndent();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        autoindent();
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void TextEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void TextEditor::focusInEvent(QFocusEvent* event) {
    emit focusChanged(event->gotFocus());
    QPlainTextEdit::focusInEvent(event);
}

void TextEditor::focusOutEvent(QFocusEvent* event) {
    emit focusChanged(event->gotFocus());
    QPlainTextEdit::focusInEvent(event);
}

void TextEditor::updateLineNumberAreaWidth(int newBlockCount [[maybe_unused]]) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Const::Color::Line);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void TextEditor::readFile() {
    QFile file(m_filePath);

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file for reading" << m_filePath;
    }
}

QString TextEditor::textUnderCursor() const {
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

int TextEditor::leftMargin() const {
    return viewportMargins().left();
}
