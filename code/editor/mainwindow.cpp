#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "projectmanager.h"

#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "neonwidget.h"

#include <Windows.h>
#include <NeonEngine.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , projectManager(new ProjectManager)
{
    ui->setupUi(this);

    auto mdi = findChild<QMdiArea*>("workspace");
    auto sb = new QMdiSubWindow(mdi);
    auto sys = new NeonWidget(sb);
    sb->setWidget(sys);
    sb->setWindowTitle("Scene view");
    projectManager->initSystem(mdi, sys, sys->GetEngine());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete projectManager;
}

void MainWindow::on_actionExit_Editor_triggered()
{
    saveAndExitProject();
}

void MainWindow::on_actionOpen_Project_triggered()
{
    projectManager->OpenProject("w:\\neon86\\data");
}

void MainWindow::on_actionSave_All_triggered()
{
    projectManager->SaveProject();
}

void MainWindow::on_MainWindow_destroyed()
{
    saveAndExitProject();
}

void MainWindow::saveAndExitProject()
{
    if (projectManager->ProjectNeedsSaving())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Save before quitting", "Do you want to save the project before quitting?",
                                    QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            projectManager->SaveProject();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    QApplication::quit();
}

void MainWindow::on_actionPlay_Pause_triggered()
{
    VM->Play();
}

void MainWindow::on_actionStop_triggered()
{
    VM->Stop();
}

void MainWindow::on_actionRestart_triggered()
{
    VM->Restart();
}
