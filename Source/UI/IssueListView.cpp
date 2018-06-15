#include "IssueListView.h"
#include "IssueModel.h"
#include "Core/Constants.h"
#include "Core/Global.h"
#include <QtGui>

static const int MARGIN = 2;

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
        painter->setBrush(opt.palette.background().color());
        backgroundColor = opt.palette.background().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    // Icon
    QFont fontIcon = Global::getFontAwesomeFont();
    int iconHeight = opt.rect.height() - MARGIN * 2;
    fontIcon.setPixelSize(iconHeight);
    painter->setFont(fontIcon);

    QString levelIcon;
    QColor levelColor;

    QString level = index.data(static_cast<int>(IssueModel::Role::Level)).toString();

    if (level == "error") {
        levelIcon = Constants::FontAwesome::TIMES_CIRCLE;
        levelColor = QColor(Constants::Color::ERROR_ISSUME_ICON);

    } else if (level == "warning") {
        levelIcon = Constants::FontAwesome::EXCLAMATION_TRIANGLE;
        levelColor = QColor(Constants::Color::WARNING_ISSUME_ICON);
    }

    QFontMetrics fmIcon(fontIcon);
    int iconWidth = iconHeight;

    painter->setPen(levelColor);
    painter->setClipRect(opt.rect);
    painter->drawText(MARGIN, fmIcon.ascent() + y + MARGIN, levelIcon);

    // Text color
    QColor textColor;
    if (selected) {
        textColor = opt.palette.highlightedText().color();
    } else {
        textColor = opt.palette.text().color();
    }

    painter->setPen(textColor);
    painter->setFont(opt.font);

    QFontMetrics fmText(opt.font);

    // Message
    if (!selected) {
        painter->setClipRect(opt.rect);
        QString message = index.data(static_cast<int>(IssueModel::Role::Message)).toString();
        painter->drawText(iconWidth + MARGIN * 3, fmText.ascent() + y, message);
    } else {
        painter->setClipRect(opt.rect);
        QString rendered = index.data(static_cast<int>(IssueModel::Role::Rendered)).toString();
        QRect rect = QRect(iconWidth + MARGIN * 3, y, opt.rect.width(), 100);
        painter->drawText(rect, Qt::AlignLeft, rendered);
    }

    // Filename
    QString filename = index.data(static_cast<int>(IssueModel::Role::Filename)).toString();
    QString line = index.data(static_cast<int>(IssueModel::Role::Line)).toString();
    QString column = index.data(static_cast<int>(IssueModel::Role::Column)).toString();
    QString filenameWithPos = QString("%1 %2:%3").arg(filename).arg(line).arg(column);
    painter->drawText(opt.rect.width() - fmText.width(filenameWithPos), fmText.ascent() + y, filenameWithPos);

    // Separator lines
    painter->setPen(QColor(Constants::Color::ISSUE_SEPARATOR));
    const QRectF borderRect = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
    painter->drawLine(borderRect.bottomLeft(), borderRect.bottomRight());

    painter->restore();
}

QSize IssueDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    if (selected) {
        return QSize(opt.rect.width(), 100);
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

IssueListView::IssueListView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
