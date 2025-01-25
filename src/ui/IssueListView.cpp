#include "IssueListView.h"
#include "IssueModel.h"
#include "core/Constants.h"
#include "core/Settings.h"
#include "core/Global.h"
#include <QPainter>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>

auto constexpr MARGIN = 2;
Q_GLOBAL_STATIC(QFont, issueFont);

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void IssueDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    int y = opt.rect.y();
    painter->save();

    const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    // Background color
    QColor backgroundColor;
    if (selected) {
        painter->setBrush(opt.palette.highlight().color());
        backgroundColor = opt.palette.highlight().color();
    } else {
        painter->setBrush(opt.palette.window().color());
        backgroundColor = opt.palette.window().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    QFontMetrics fmText(*issueFont);

    // Icon
    QFont fontIcon = Global::fontAwesomeFont();
    int iconHeight = fmText.height() - MARGIN * 2;
    int iconWidth = iconHeight;
    fontIcon.setPixelSize(iconHeight);
    painter->setFont(fontIcon);

    QString levelIcon;
    QColor levelColor;

    QString level = index.data(static_cast<int>(IssueModel::Role::Level)).toString();

    if (level == "error") {
        levelIcon = Const::FontAwesome::TimesCircle;
        levelColor = QColor(Const::Color::ErrorIssueIcon);
    } else if (level == "warning") {
        levelIcon = Const::FontAwesome::ExclamationTriangle;
        levelColor = QColor(Const::Color::WarningIssueIcon);
    }

    painter->setPen(levelColor);
    painter->setClipRect(opt.rect);
    painter->drawText(MARGIN, y + iconHeight, levelIcon);

    // Text color
    QColor textColor;

    if (selected) {
        textColor = opt.palette.highlightedText().color();
    } else {
        textColor = opt.palette.text().color();
    }

    painter->setPen(textColor);
    painter->setFont(*issueFont);

    // Message
    if (!selected) {
        painter->setClipRect(opt.rect);
        QString message = index.data(static_cast<int>(IssueModel::Role::Message)).toString();
        painter->drawText(iconWidth + MARGIN * 3, fmText.ascent() + y, message);
    } else {
        painter->setClipRect(opt.rect);
        QString rendered = index.data(static_cast<int>(IssueModel::Role::Rendered)).toString();
        QRect rect = QRect(iconWidth + MARGIN * 3, y, opt.rect.width(), opt.rect.height());
        painter->drawText(rect, Qt::AlignLeft, rendered);
    }

    // Filename
    QString filename = index.data(static_cast<int>(IssueModel::Role::Filename)).toString();

    if (!filename.isEmpty()) {
        QString line = index.data(static_cast<int>(IssueModel::Role::Line)).toString();
        QString column = index.data(static_cast<int>(IssueModel::Role::Column)).toString();
        QString filenameWithPos = QString("%1 %2:%3").arg(filename, line, column);
        painter->drawText(opt.rect.width() - fmText.horizontalAdvance(filenameWithPos) - MARGIN, fmText.ascent() + y, filenameWithPos);
    }

    // Separator lines
    painter->setPen(QColor(Const::Color::IssueSeparator));
    const QRectF borderRect = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
    painter->drawLine(borderRect.bottomLeft(), borderRect.bottomRight());

    painter->restore();
}

QSize IssueDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QFontMetrics fm(*issueFont);

    const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    if (selected) {
        QString rendered = index.data(static_cast<int>(IssueModel::Role::Rendered)).toString();
        QStringList rows = rendered.split('\n', Qt::SkipEmptyParts);
        return QSize(opt.rect.width(), fm.height() * (rows.count()) + 3);
    } else {
        return QSize(opt.rect.width(), fm.height());
    }
}

void IssueDelegate::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    emit sizeHintChanged(current);
    emit sizeHintChanged(previous);
}

IssueListView::IssueListView(IssueModel* model, QWidget* parent) : QListView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    const QString& family = Settings::value("gui.output.issues.font.family").toString();
    int size = Settings::value("gui.output.issues.font.size").toInt();
    *issueFont = QFont(family, size);

    setModel(model);

    IssueDelegate* delegate = new IssueDelegate(this);
    setItemDelegate(delegate);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, delegate, &IssueDelegate::currentChanged);

    connect(this, &QListView::customContextMenuRequested, this, &IssueListView::onCustomContextMenu);

    m_contextMenu = new QMenu(this);
    QAction* copyLabelAction = m_contextMenu->addAction(tr("Copy Label"));
    connect(copyLabelAction, &QAction::triggered, this, &IssueListView::onCopyLabelAction);

    QAction* copyDescriptionAction = m_contextMenu->addAction(tr("Copy Description"));
    connect(copyDescriptionAction, &QAction::triggered, this, &IssueListView::onCopyDescriptionAction);
}

void IssueListView::onCustomContextMenu(const QPoint& point) {
    if (indexAt(point).isValid()) {
        m_contextMenu->exec(mapToGlobal(point));
    }
}

void IssueListView::onCopyLabelAction() {
    copyRowToClipboard(static_cast<int>(IssueModel::Role::Message));
}

void IssueListView::onCopyDescriptionAction() {
    copyRowToClipboard(static_cast<int>(IssueModel::Role::Rendered));
}

void IssueListView::copyRowToClipboard(int role) {
    QModelIndex index = selectionModel()->selectedIndexes().first();
    QString text = model()->data(index, role).toString();
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}
