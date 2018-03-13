#include "Editor.h"

Editor::Editor(QString filePath, QWidget* parent) :
    QTextEdit(parent),
    filePath(filePath) {

}
