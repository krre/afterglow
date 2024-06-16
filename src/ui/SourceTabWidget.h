#pragma once
#include <QTabWidget>

class SourceTabWidget : public QTabWidget {
public:
    SourceTabWidget(QWidget* parent = nullptr);

    int findTab(const QString& filePath);

public slots:
    void saveAll();

    void closeFile(const QString& filePath);
    void closeTab(int index);
    void closeCurrentTab();
    void closeAll();
    void closeOthers();
};
