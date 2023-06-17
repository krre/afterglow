#include "CoProcess.h"
#include <QDebug>

CoProcess::CoProcess() {

}

CoProcess::~CoProcess() {
}

CoAwaiter CoProcess::startAsync(const QString& program, const QStringList& arguments, QIODeviceBase::OpenMode mode) {
    start(program, arguments, mode);
    return CoAwaiter{};
}
