#include "SourceTabWidget.h"
#include "texteditor/TextEditor.h"

SourceTabWidget::SourceTabWidget(QWidget* parent) : QTabWidget(parent) {
    setMinimumSize(QSize(0, 50));
    setTabsClosable(true);
    setMovable(true);

    connect(this, &QTabWidget::tabCloseRequested, this, &SourceTabWidget::closeTab);
}

int SourceTabWidget::findTab(const QString& filePath) {
    for (int i = 0; i < count(); ++i) {
        if (static_cast<TextEditor*>(widget(i))->filePath() == filePath) {
            return i;
        }
    }

    return -1;
}

void SourceTabWidget::saveAll() {
    for (int i = 0; i < count(); ++i) {
        static_cast<TextEditor*>(widget(i))->saveFile();
    }
}

void SourceTabWidget::closeFile(const QString& filePath) {
    QVector<int> indices;

    for (int i = 0; i < count(); ++i) {
        if (static_cast<TextEditor*>(widget(i))->filePath().contains(filePath)) {
            indices.append(i);
        }
    }

    // Remove tabs in reverse order
    for (int i = indices.count() - 1; i >= 0; --i) {
        closeTab(indices.at(i));
    }
}

void SourceTabWidget::closeTab(int index) {
    QWidget* w = widget(index);
    removeTab(index);
    delete w;
}

void SourceTabWidget::closeCurrentTab() {
    closeTab(currentIndex());
}

void SourceTabWidget::closeAll() {
    while (count()) {
        closeTab(0);
    }
}

void SourceTabWidget::closeOthers() {
    int i = 0;

    while (count() > 1) {
        if (i != currentIndex()) {
            closeTab(i);
        } else {
            ++i;
        }
    }
}
