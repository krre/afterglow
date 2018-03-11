#include "CargoProcessor.h"
#include <QtCore>

CargoProcessor::CargoProcessor(QObject* parent) : QObject(parent) {

}

CargoProcessor::~CargoProcessor() {

}

void CargoProcessor::createProject(const QString& command) {
    qDebug() << command;
}
