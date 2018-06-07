#include "IssueListView.h"
#include "IssueModel.h"
#include "Core/Constants.h"
#include <QtGui>

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void IssueDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->save();

    auto view = qobject_cast<const QAbstractItemView*>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    // Background color
    QColor backgroundColor;
    if (selected) {
        painter->setBrush(opt.palette.highlight().color());
        backgroundColor = opt.palette.highlight().color();
    } else {
        painter->setBrush(opt.palette.background().color());
        backgroundColor = opt.palette.background().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    // Text color
    QColor textColor;
    if (selected) {
        textColor = opt.palette.highlightedText().color();
    } else {
        textColor = opt.palette.text().color();
    }

    painter->setPen(textColor);

    QFontMetrics fm(opt.font);
    if (!selected) {
        painter->setClipRect(opt.rect);
        painter->drawText(0, 0 + fm.ascent(), index.data(static_cast<int>(IssueModel::Role::Message)).toString());
    } else {

    }

    // Separator lines
    painter->setPen(QColor(Constants::Color::ISSUE_SEPARATOR));
    const QRectF borderRect = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
    painter->drawLine(borderRect.bottomLeft(), borderRect.bottomRight());
    painter->restore();
}

QSize IssueDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}

IssueListView::IssueListView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
