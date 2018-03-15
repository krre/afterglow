#include "ProjectTreeView.h"
#include <QtWidgets>

ProjectTreeView::ProjectTreeView(QWidget* parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenu);
    connect(this, &QTreeView::doubleClicked, this, &ProjectTreeView::onDoubleClicked);

    fsModel = new QFileSystemModel(this);

    contextMenu = new QMenu(this);
    QAction* openAction = contextMenu->addAction(tr("Open"));
    connect(openAction, &QAction::triggered, [=]() {
        QModelIndex index = selectedIndexes().first();
        if (fsModel->isDir(index)) {
            setExpanded(index, true);
        } else {
            openActivated(fsModel->filePath(index));
        }
    });
    QAction* removeAction = contextMenu->addAction(tr("Remove..."));
    connect(removeAction, &QAction::triggered, this, &ProjectTreeView::onFileRemove);
    QAction* renameAction = contextMenu->addAction(tr("Rename..."));
    connect(renameAction, &QAction::triggered, this, &ProjectTreeView::onFileRename);

    setFrameShape(QFrame::NoFrame);
    setHeaderHidden(true);
}

ProjectTreeView::~ProjectTreeView() {
}

void ProjectTreeView::setRootPath(const QString& path) {
    if (path.isNull()) {
        setModel(nullptr);
    } else {
        setModel(fsModel);
        setRootIndex(fsModel->setRootPath(path));

        // Hide columns exclude first
        for (int i = 1; i < fsModel->columnCount(); ++i) {
            hideColumn(i);
        }
    }
}

void ProjectTreeView::onCustomContextMenu(const QPoint& point) {
    QModelIndex index = indexAt(point);
    if (index.isValid()) {
        contextMenu->exec(mapToGlobal(point));
    }
}

void ProjectTreeView::onDoubleClicked(const QModelIndex& index) {
    QFileInfo fi = qobject_cast<QFileSystemModel*>(model())->fileInfo(index);
    if (!fi.isDir()) {
        emit openActivated(fi.absoluteFilePath());
    }
}

void ProjectTreeView::onFileRemove() {
    QModelIndex index = selectedIndexes().first();
    QString text = QString("Remove %1 \"%2\"?")
            .arg(fsModel->isDir(index) ? tr("directory") : tr("file"))
            .arg(fsModel->fileName(index));
    int result = QMessageBox::question(this, tr("Remove"), text);
    if (result == QMessageBox::Yes) {
        emit removeActivated(fsModel->filePath(index));
    }
}

void ProjectTreeView::onFileRename() {
    QModelIndex index = selectedIndexes().first();
    emit renameActivated(fsModel->filePath(index));
}

