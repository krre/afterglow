#include "Editor.h"
#include "LineNumberArea.h"
#include "Highlighter.h"
#include "Core/Settings.h"
#include "Core/Constants.h"
#include <QtGui>

Editor::Editor(QString filePath, QWidget* parent) :
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

    connect(this, &Editor::blockCountChanged, this, &Editor::updateLineNumberAreaWidth);
    connect(this, &Editor::updateRequest, this, &Editor::updateLineNumberArea);
    connect(this, &Editor::cursorPositionChanged, this, &Editor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    connect(this, &Editor::textChanged, [=](){
        emit documentModified(this);
    });

    readFile();
}

void Editor::setFilePath(const QString& filePath) {
    this->filePath = filePath;
}

void Editor::saveFile() {
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

QString Editor::getModifiedName() const {
    QFileInfo fi(filePath);
    return fi.fileName() + (document()->isModified() ? "*" : "");
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent* event) {
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

int Editor::getLineNumberAreaWidth() {
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

void Editor::commentUncommentLine() {
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

void Editor::joinLines() {
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

void Editor::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Tab) {
        // Add white spaces to right
        insertPlainText(QString(Constants::TAB_SPACES_COUNT, ' '));
    } else if (event->key() == Qt::Key_Backtab) {
        // Remove white spaces to left
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
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Autoindent
        QPlainTextEdit::keyPressEvent(event);
        QTextCursor cursor = textCursor();
        int row = cursor.blockNumber();
//        int column = cursor.positionInBlock();
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
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void Editor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), getLineNumberAreaWidth(), cr.height()));
}

void Editor::updateLineNumberAreaWidth(int newBlockCount) {
    Q_UNUSED(newBlockCount)
    setViewportMargins(getLineNumberAreaWidth(), 0, 0, 0);
}

void Editor::highlightCurrentLine() {
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

void Editor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void Editor::readFile() {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file for reading" << filePath;
    }
}
