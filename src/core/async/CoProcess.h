#pragma once
#include "CoTask.h"
#include <QProcess>

class CoProcess : public QProcess {
public:
    CoProcess();
    ~CoProcess();
    CoAwaiter startAsync(const QString& program, const QStringList& arguments = {}, QIODeviceBase::OpenMode mode = ReadWrite);
};

