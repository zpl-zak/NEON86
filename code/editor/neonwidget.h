#ifndef NEONWIDGET_H
#define NEONWIDGET_H

#include <QWidget>

class CEngine;
class QTimer;

class NeonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NeonWidget(QWidget *parent = nullptr);
    ~NeonWidget();

    CEngine* GetEngine();
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    QPaintEngine * paintEngine() const override { return NULL; }

private slots:
    void initAfterWarmup();
    void updateScene();

private:
    CEngine* engine;
    QTimer* updatePoll;
};

#endif // NEONWIDGET_H
