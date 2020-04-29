#include "projectmanager.h"
#include "neonwidget.h"

#include <Windows.h>

#include <NeonEngine.h>

#include <QMessageBox>
#include <QMdiSubWindow>
#include "codeeditor.h"

ProjectManager::ProjectManager()
{

}

void ProjectManager::OpenProject(QString path)
{
    if (!FILESYSTEM->LoadGame((LPSTR)path.toStdString().c_str()))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(sceneView, "Filesystem error", "Could not load game dir");

        return;
    }

    auto scriptView = new QMdiSubWindow(mdi);
    scriptEditor = new CodeEditor(scriptView);
    scriptView->setWidget(scriptEditor);
    scriptView->show();
    scriptView->setWindowTitle("Lua scripting");
}

void ProjectManager::SaveProject()
{

}

void ProjectManager::CloseProject()
{

}

bool ProjectManager::ProjectNeedsSaving()
{
    return true;
}

void ProjectManager::initSystem(QMdiArea* mdi, QWidget *sysW, CEngine *sys)
{
    this->engine = sys;
    this->sceneView = sysW;
    this->mdi = mdi;
}
