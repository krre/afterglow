#pragma once
#include <QTreeView>
#include <QFileSystemModel>

class QMenu;

class ProjectTreeView : public QTreeView {
    Q_OBJECT

public:
    explicit ProjectTreeView(QWidget* parent = nullptr);
    ~ProjectTreeView();

    void setRootPath(const QString& path);

signals:
    void openActivated(const QString& filePath);
    void removeActivated(const QString& filePath);

private slots:
    void onMousePressed(const QModelIndex& index);
    void onDoubleClicked(const QModelIndex& index);
    void onFileRemove();
    void onFileRename();

private:
    QFileSystemModel* fsModel;
    QMenu* contextMenu;
    QString selectedFile;

};
