#include <QAbstractItemModel>

class IssuesModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssuesModel(QObject* parent = nullptr);

signals:

};
