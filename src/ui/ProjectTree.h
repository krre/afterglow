#pragma once
#include <QTreeView>
#include <QFileSystemModel>

class QMenu;
class FileSystemProxyModel;

class ProjectTree : public QTreeView {
    Q_OBJECT
public:
    explicit ProjectTree(QWidget* parent = nullptr);
    ~ProjectTree();

    void setRootPath(const QString& path);
    QFileSystemModel* getFsModel() const { return m_fsModel; }
    void selectFile(const QString& filePath);

signals:
    void openActivated(const QString& filePath);
    void removeActivated(const QString& filePath);
    void renameActivated(const QString& oldPath, const QString& newPath);
    void newFileActivated(const QString& filePath);

public slots:
    void onNewRustFile();
    void onNewFile();
    void onNewDirectory();

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onDoubleClicked(const QModelIndex& index);

    void onFileRemove();
    void onFileRename();

private:
    QString getCurrentDirectory() const;

    QFileSystemModel* m_fsModel = nullptr;
    FileSystemProxyModel* m_fsProxyModel = nullptr;
    QMenu* m_contextMenu;
};
