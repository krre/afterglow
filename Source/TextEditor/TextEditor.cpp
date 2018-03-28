#include "TextEditor.h"
#include "LineNumberArea.h"
#include "Highlighter.h"
#include "Core/Settings.h"
#include "Core/Constants.h"
#include <QtWidgets>

TextEditor::TextEditor(QString filePath, QWidget* parent) :
        QPlainTextEdit(parent),
        filePath(filePath) {
    setFrameShape(QFrame::NoFrame);

    const QString& family = Settings::getValue("editor.font.family").toString();
    int size = Settings::getValue("editor.font.size").toInt();
    QFont font(family, size);
    document()->setDefaultFont(font);

    setWordWrapMode(QTextOption::NoWrap);

    lineNumberArea = new LineNumberArea(this);
    highlighter = new Highlighter(document());

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
    this->filePath = filePath;
}

void TextEditor::setCompleter(QCompleter* completer) {
    if (this->completer)
        QObject::disconnect(completer, 0, this, 0);

    this->completer = completer;

    if (!completer)
        return;

    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void TextEditor::saveFile() {
    if (!document()->isModified()) return;

    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toPlainText();
        document()->setModified(false);
        documentModified(this);
    } else {
        qWarning() << "Failed to open file for writing" << filePath;
    }
}

QString TextEditor::getModifiedName() const {
    QFileInfo fi(filePath);
    return fi.fileName() + (document()->isModified() ? "*" : "");
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(170, 170, 170));
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int TextEditor::getLineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    const int DEFAULT_DIGITS = 4;
    digits = qMax(DEFAULT_DIGITS, digits);

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void TextEditor::commentUncommentLine() {
    QTextCursor cursor = textCursor();
    int row = cursor.blockNumber();
    QTextBlock block = document()->findBlockByLineNumber(row);
    if (!block.text().size()) return;

    cursor.beginEditBlock();

    int i = 0;
    while (block.text().at(i) == ' ') {
        i++;
    };

    if (block.text().at(i) == '/' && block.text().at(i + 1) == '/') {
        // Uncomment
        cursor.setPosition(block.position() + i);
        cursor.deleteChar();
        cursor.deleteChar();
    } else {
        // Comment
        cursor.setPosition(block.position());
        cursor.insertText("//");
    }

    cursor.endEditBlock();
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
    cutLine.replace(QRegExp(QLatin1String("^\\s*")), QLatin1String(" "));
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    cursor.movePosition(QTextCursor::PreviousBlock);
    cursor.movePosition(QTextCursor::EndOfBlock);

    cursor.insertText(cutLine);

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

// Add white spaces to right
void TextEditor::insertTabSpaces() {
    insertPlainText(QString(Constants::TAB_SPACES_COUNT, ' '));
}

// Remove white spaces to left
void TextEditor::removeTabSpaces() {
    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    int charPos = cursor.position() - block.position();
    if (charPos && block.text().at(charPos - 1) == ' ') {
        int removeSpaces = charPos % Constants::TAB_SPACES_COUNT;
        if (!removeSpaces) {
            removeSpaces = Constants::TAB_SPACES_COUNT;
        }

        while (removeSpaces) {
            cursor.movePosition(QTextCursor::PreviousCharacter);
            cursor.deleteChar();
            charPos = cursor.position() - block.position();
            if (charPos && block.text().at(charPos - 1) != ' ') {
                break;
            }
            removeSpaces--;
        }
    }
}

void TextEditor::autocomplete(QKeyEvent* event) {
    bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space);
    if (!completer || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit(event);

    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!completer || (ctrlOrShift && event->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(event->text().right(1)))) {
        completer->popup()->hide();
        return;
    }

    if (completionPrefix != completer->completionPrefix()) {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setX(cr.x() + viewportMargins().left());
    cr.setWidth(completer->popup()->sizeHintForColumn(0)
                + completer->popup()->verticalScrollBar()->sizeHint().width());
    completer->complete(cr); // popup it up!
}

void TextEditor::keyPressEvent(QKeyEvent* event) {
    if (completer && completer->popup()->isVisible()) {
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
        insertTabSpaces();
    } else if (event->key() == Qt::Key_Backtab) {
        removeTabSpaces();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        autoindent();
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        autocomplete(event);
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void TextEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), getLineNumberAreaWidth(), cr.height()));
}

void TextEditor::focusInEvent(QFocusEvent* event) {
    if (event->gotFocus() && completer) {
        completer->setWidget(this);
    }
    QPlainTextEdit::focusInEvent(event);
}

void TextEditor::updateLineNumberAreaWidth(int newBlockCount) {
    Q_UNUSED(newBlockCount)
    setViewportMargins(getLineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

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
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void TextEditor::readFile() {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file for reading" << filePath;
    }
}

QString TextEditor::textUnderCursor() const {
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void TextEditor::insertCompletion(const QString& completion) {
    if (completer->widget() != this) {
        return;
    }

    QTextCursor cursor = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(completion.right(extra));
    setTextCursor(cursor);
}
