#include "ProjectTree.h"
#include "FileSystemProxyModel.h"
#include "NewName.h"
#include "Rename.h"
#include <QtWidgets>

ProjectTree::ProjectTree(QWidget* parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &ProjectTree::onCustomContextMenu);
    connect(this, &QTreeView::doubleClicked, this, &ProjectTree::onDoubleClicked);
    
    m_fsModel = new QFileSystemModel(this);
    m_fsProxyModel = new FileSystemProxyModel(this);
    m_fsProxyModel->setSourceModel(m_fsModel);
    m_fsProxyModel->sort(0, Qt::AscendingOrder);
    
    m_contextMenu = new QMenu(this);

    QMenu* newMenu = new QMenu(tr("New"), this);

    QAction* newRustFileAction = newMenu->addAction(tr("Rust File..."));
    connect(newRustFileAction, &QAction::triggered, this, &ProjectTree::onNewRustFile);

    QAction* newFileAction = newMenu->addAction(tr("File..."));
    connect(newFileAction, &QAction::triggered, this, &ProjectTree::onNewFile);

    QAction* newDirectoryAction = newMenu->addAction(tr("Directory..."));
    connect(newDirectoryAction, &QAction::triggered, this, &ProjectTree::onNewDirectory);
    
    m_contextMenu->addMenu(newMenu);
    
    QAction* openAction = m_contextMenu->addAction(tr("Open"));
    connect(openAction, &QAction::triggered, this, [=, this] () {
        QModelIndex proxyIndex = selectedIndexes().constFirst();
        QModelIndex sourceIndex = m_fsProxyModel->mapToSource(proxyIndex);
        if (m_fsModel->isDir(sourceIndex)) {
            setExpanded(proxyIndex, true);
        } else {
            emit openActivated(m_fsModel->filePath(sourceIndex));
        }
    });
    
    QAction* removeAction = m_contextMenu->addAction(tr("Remove..."));
    connect(removeAction, &QAction::triggered, this, &ProjectTree::onFileRemove);
    
    QAction* renameAction = m_contextMenu->addAction(tr("Rename..."));
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
        setModel(m_fsProxyModel);
        QModelIndex index = m_fsModel->setRootPath(path);
        setRootIndex(m_fsProxyModel->mapFromSource(index));

        // Hide columns exclude first
        for (int i = 1; i < m_fsModel->columnCount(); ++i) {
            hideColumn(i);
        }
    }
}

void ProjectTree::selectFile(const QString& filePath) {
    QModelIndex index = m_fsModel->index(filePath);
    setCurrentIndex(m_fsProxyModel->mapFromSource(index));
}

void ProjectTree::onCustomContextMenu(const QPoint& point) {
    if (indexAt(point).isValid()) {
        m_contextMenu->exec(mapToGlobal(point));
    }
}

void ProjectTree::onDoubleClicked(const QModelIndex& index) {
    QFileInfo fi = m_fsModel->fileInfo(m_fsProxyModel->mapToSource(index));
    if (!fi.isDir()) {
        emit openActivated(fi.absoluteFilePath());
    }
}

void ProjectTree::onNewRustFile() {
    NewName newName(tr("New Rust File"));
    if (newName.exec() == QDialog::Rejected) return;

    QFileInfo fi(newName.name());
    QString newFilePath = getCurrentDirectory() + "/" + fi.baseName() + ".rs";
    emit newFileActivated(newFilePath);
}

void ProjectTree::onNewFile() {
    NewName newName(tr("New File"));
    if (newName.exec() == QDialog::Rejected) return;
    emit newFileActivated(getCurrentDirectory() + "/" + newName.name());
}

void ProjectTree::onNewDirectory() {
    NewName newName(tr("New Directory"));
    if (newName.exec() == QDialog::Rejected) return;
    
    QModelIndex sourceIndex = m_fsProxyModel->mapToSource(selectedIndexes().constFirst());
    QModelIndex index = m_fsModel->mkdir(m_fsModel->isDir(sourceIndex) ? sourceIndex : m_fsModel->parent(sourceIndex), newName.name());
    setCurrentIndex(index);
}

void ProjectTree::onFileRemove() {
    QModelIndex index = m_fsProxyModel->mapToSource(selectedIndexes().constFirst());
    bool isDir = m_fsModel->isDir(index);
    QString text = QString("Remove %1 \"%2\"?")
                       .arg(isDir ? tr("directory") : tr("file"), m_fsModel->fileName(index));
    int result = QMessageBox::question(this, tr("Remove"), text);
    if (result == QMessageBox::Yes) {
        QString path = m_fsModel->filePath(index);
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
    QString oldPath = m_fsModel->filePath(m_fsProxyModel->mapToSource(selectedIndexes().constFirst()));
    QFileInfo fi(oldPath);

    Rename rename(fi.fileName(), this);
    if (rename.exec() == QDialog::Rejected) return;

    QString newPath = (fi.isDir() ? fi.absolutePath() : fi.path()) + "/" + rename.name();
    QDir dir;

    if (dir.rename(oldPath, newPath)) {
        setCurrentIndex(m_fsProxyModel->mapFromSource(m_fsModel->index(newPath)));
        emit renameActivated(oldPath, newPath);
    } else {
        qWarning() << QString("Failed to rename %1 to %2").arg(oldPath, newPath);
    }
}

QString ProjectTree::getCurrentDirectory() const {
    QModelIndex index = m_fsProxyModel->mapToSource(selectedIndexes().constFirst());
    return m_fsModel->isDir(index) ? m_fsModel->filePath(index)
                                   : m_fsModel->fileInfo(index).absolutePath();
}
