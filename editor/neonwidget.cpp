#include "neonwidget.h"

#include <Windows.h>
#include <NeonEngine.h>

#include <QTimer>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QMessageBox>

NeonWidget::NeonWidget(QWidget *parent)
    : QWidget(parent)
    , engine(new CEngine)
    , updatePoll(new QTimer)
{
    parent->setAttribute(Qt::WA_DeleteOnClose, true);
    parent->setFixedSize(640, 480);
    parent->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowSystemMenuHint);

    updatePoll->setInterval(1000);
    updatePoll->setSingleShot(true);
    connect(updatePoll, SIGNAL(timeout()), SLOT(initAfterWarmup()));
    updatePoll->start();
}

NeonWidget::~NeonWidget()
{
    delete updatePoll;

    engine->Shutdown();
    delete engine;
}

CEngine *NeonWidget::GetEngine()
{
    return engine;
}

void NeonWidget::paintEvent(QPaintEvent* /*event*/)
{
    if (engine->IsRunning())
        engine->Think();
}

void NeonWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void NeonWidget::initAfterWarmup()
{
    QRect rect = geometry();
    RECT res;

    res.left = rect.left();
    res.top = rect.top();
    res.right = rect.right();
    res.bottom = rect.bottom();

    if (!engine->Init((HWND)winId(), res))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Engine error", "Could not init engine!");

        QCoreApplication::quit();
    }

    delete updatePoll;
    updatePoll = new QTimer();
    updatePoll->setInterval(1);
    updatePoll->setSingleShot(false);
    connect(updatePoll, SIGNAL(timeout()), SLOT(updateScene()));
    updatePoll->start();
}

void NeonWidget::updateScene()
{
    if (engine->IsRunning())
        engine->Think();
}
