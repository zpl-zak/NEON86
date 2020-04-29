#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ProjectManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_Editor_triggered();

    void on_actionOpen_Project_triggered();

    void on_actionSave_All_triggered();

    void on_MainWindow_destroyed();

    void on_actionPlay_Pause_triggered();

    void on_actionStop_triggered();

    void on_actionRestart_triggered();

private:
    Ui::MainWindow *ui;
    ProjectManager *projectManager;

    void saveAndExitProject();
};
#endif // MAINWINDOW_H
