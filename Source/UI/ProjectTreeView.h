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
    QFileSystemModel* getFsModel() const { return fsModel; }

signals:
    void openActivated(const QString& filePath);
    void removeActivated(const QString& filePath);
    void renameActivated(const QString& filePath);
    void newFileActivated(const QString& filePath);

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onDoubleClicked(const QModelIndex& index);

    void onNewRustFile();
    void onNewFile();
    void onNewDirectory();

    void onFileRemove();
    void onFileRename();

private:
    QString getCurrentDirectory() const;

    QFileSystemModel* fsModel;
    QMenu* contextMenu;
};
