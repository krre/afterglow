#include "ProjectTree.h"
#include "FileSystemProxyModel.h"
#include "NewName.h"
#include "Rename.h"
#include <QtWidgets>

ProjectTree::ProjectTree(QWidget* parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &ProjectTree::onCustomContextMenu);
    connect(this, &QTreeView::doubleClicked, this, &ProjectTree::onDoubleClicked);

    fsModel = new QFileSystemModel(this);
    fsProxyModel = new FileSystemProxyModel(this);
    fsProxyModel->setSourceModel(fsModel);
    fsProxyModel->sort(0, Qt::AscendingOrder);

    contextMenu = new QMenu(this);

    QMenu* newMenu = new QMenu(tr("New"), this);

    QAction* newRustFileAction = newMenu->addAction(tr("Rust File..."));
    connect(newRustFileAction, &QAction::triggered, this, &ProjectTree::onNewRustFile);

    QAction* newFileAction = newMenu->addAction(tr("File..."));
    connect(newFileAction, &QAction::triggered, this, &ProjectTree::onNewFile);

    QAction* newDirectoryAction = newMenu->addAction(tr("Directory..."));
    connect(newDirectoryAction, &QAction::triggered, this, &ProjectTree::onNewDirectory);

    contextMenu->addMenu(newMenu);

    QAction* openAction = contextMenu->addAction(tr("Open"));
    connect(openAction, &QAction::triggered, [=]() {
        QModelIndex proxyIndex = selectedIndexes().first();
        QModelIndex sourceIndex = fsProxyModel->mapToSource(proxyIndex);
        if (fsModel->isDir(sourceIndex)) {
            setExpanded(proxyIndex, true);
        } else {
            openActivated(fsModel->filePath(sourceIndex));
        }
    });

    QAction* removeAction = contextMenu->addAction(tr("Remove..."));
    connect(removeAction, &QAction::triggered, this, &ProjectTree::onFileRemove);

    QAction* renameAction = contextMenu->addAction(tr("Rename..."));
    connect(renameAction, &QAction::triggered, this, &ProjectTree::onFileRename);

    setFrameShape(QFrame::NoFrame);
    setHeaderHidden(true);
}

ProjectTree::~ProjectTree() {
}

void ProjectTree::setRootPath(const QString& path) {
    if (path.isNull()) {
        setModel(nullptr);
    } else {
        setModel(fsProxyModel);
        QModelIndex index = fsModel->setRootPath(path);
        setRootIndex(fsProxyModel->mapFromSource(index));

        // Hide columns exclude first
        for (int i = 1; i < fsModel->columnCount(); ++i) {
            hideColumn(i);
        }
    }
}

void ProjectTree::selectFile(const QString& filePath) {
    QModelIndex index = fsModel->index(filePath);
    setCurrentIndex(fsProxyModel->mapFromSource(index));
}

void ProjectTree::onCustomContextMenu(const QPoint& point) {
    if (indexAt(point).isValid()) {
        contextMenu->exec(mapToGlobal(point));
    }
}

void ProjectTree::onDoubleClicked(const QModelIndex& index) {
    QFileInfo fi = fsModel->fileInfo(fsProxyModel->mapToSource(index));
    if (!fi.isDir()) {
        emit openActivated(fi.absoluteFilePath());
    }
}

void ProjectTree::onNewRustFile() {
    NewName newName(tr("New Rust File"), this);
    newName.exec();
    QString name = newName.getName();
    if (!name.isEmpty()) {
        QFileInfo fi(name);
        QString newFilePath = getCurrentDirectory() + "/" + fi.baseName() + ".rs";
        newFileActivated(newFilePath);
    }
}

void ProjectTree::onNewFile() {
    NewName newName(tr("New File"), this);
    newName.exec();
    QString name = newName.getName();
    if (!name.isEmpty()) {
        newFileActivated(getCurrentDirectory() + "/" + name);
    }
}

void ProjectTree::onNewDirectory() {
    NewName newName(tr("New Directory"), this);
    newName.exec();
    QString name = newName.getName();
    if (!name.isEmpty()) {
        QModelIndex sourceIndex = fsProxyModel->mapToSource(selectedIndexes().first());
        QModelIndex index = fsModel->mkdir(fsModel->isDir(sourceIndex) ? sourceIndex : fsModel->parent(sourceIndex), name);
        setCurrentIndex(index);
    }
}

void ProjectTree::onFileRemove() {
    QModelIndex index = fsProxyModel->mapToSource(selectedIndexes().first());
    bool isDir = fsModel->isDir(index);
    QString text = QString("Remove %1 \"%2\"?")
            .arg(isDir ? tr("directory") : tr("file"))
            .arg(fsModel->fileName(index));
    int result = QMessageBox::question(this, tr("Remove"), text);
    if (result == QMessageBox::Yes) {
        QString path = fsModel->filePath(index);
        QDir dir(path);
        bool success = false;
        if (isDir) {
            success = dir.removeRecursively();
        } else {
            success = dir.remove(path);
        }

        if (success) {
            emit removeActivated(path);
        } else {
            qWarning() << "Failed to remove" << path;
        }
    }
}

void ProjectTree::onFileRename() {
    QString oldPath = fsModel->filePath(fsProxyModel->mapToSource(selectedIndexes().first()));
    Rename rename(oldPath, this);
    rename.exec();
    QString name = rename.getName();
    if (!name.isEmpty()) {
        QFileInfo fi(oldPath);
        QString newPath = (fi.isDir() ? fi.absolutePath() : fi.path()) + "/" + name;
        QDir dir;
        if (dir.rename(oldPath, newPath)) {
            setCurrentIndex(fsProxyModel->mapFromSource(fsModel->index(newPath)));
            renameActivated(oldPath, newPath);
        } else {
            qWarning() << QString("Failed to rename %1 to %2").arg(oldPath).arg(newPath);
        }
    }
}

QString ProjectTree::getCurrentDirectory() const {
    QModelIndex index = fsProxyModel->mapToSource(selectedIndexes().first());
    return fsModel->isDir(index) ? fsModel->filePath(index)
                                 : fsModel->fileInfo(index).absolutePath();
}
