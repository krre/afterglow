#pragma once
#include <QMenu>

class RecentMenu : public QMenu {
    Q_OBJECT
public:
    RecentMenu(const QString& title, QWidget* parent = nullptr);

    void addRecent(const QString& recent);
    QStringList recents() const;
    void updateState();

signals:
    void activated(const QString& recent);

private slots:
    void clear();
};
