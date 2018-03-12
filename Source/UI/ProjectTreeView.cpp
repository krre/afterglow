#include "ProjectTreeView.h"
#include <QtWidgets>

ProjectTreeView::ProjectTreeView(QWidget* parent) : QTreeView(parent) {
    connect(this, &QTreeView::doubleClicked, this, &ProjectTreeView::onDoubleClicked);
    connect(this, &QTreeView::pressed, this, &ProjectTreeView::onMousePressed);

    fsModel = new QFileSystemModel(this);

    contextMenu = new QMenu(this);
    QAction* openAction = contextMenu->addAction(tr("Open"));
    connect(openAction, &QAction::triggered, [=]() { openActivated(selectedFile); });
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
    setModel(fsModel);
    setRootIndex(fsModel->setRootPath(path));

    for (int i = 1; i < fsModel->columnCount(); ++i) {
        hideColumn(i);
    }
}

void ProjectTreeView::onMousePressed(const QModelIndex& index) {
    if (QApplication::mouseButtons() == Qt::RightButton) {
        QFileInfo fi = qobject_cast<QFileSystemModel*>(model())->fileInfo(index);
        if (!fi.isDir()) {
            selectedFile = fi.absoluteFilePath();
            contextMenu->exec(QCursor::pos());
        }
    }
}

void ProjectTreeView::onDoubleClicked(const QModelIndex& index) {
    QFileInfo fi = qobject_cast<QFileSystemModel*>(model())->fileInfo(index);
    if (!fi.isDir() && fi.suffix() == "irbis") {
        emit openActivated(fi.absoluteFilePath());
    }
}

void ProjectTreeView::onFileRemove() {
    int result = QMessageBox::question(this, tr("Remove File"), tr("Are you sure?"));
    if (result == QMessageBox::Yes) {
        emit removeActivated(selectedFile);
    }
}

void ProjectTreeView::onFileRename() {
    qDebug() << "Rename" << selectedFile;
}

