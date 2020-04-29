#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QMainWindow>
#include <QMdiArea>

class CEngine;
class CodeEditor;

class ProjectManager
{
public:
    ProjectManager();
    void OpenProject(QString path);
    void SaveProject();
    void CloseProject();
    bool ProjectNeedsSaving();

    void initSystem(QMdiArea* mdi, QWidget *sysW, CEngine* sys);

private:
    CEngine* engine;
    QWidget* sceneView;
    QMdiArea* mdi;
    CodeEditor* scriptEditor;
};

#endif // PROJECTMANAGER_H
