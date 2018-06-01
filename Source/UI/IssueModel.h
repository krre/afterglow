#include <QAbstractItemModel>

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);

signals:

};
