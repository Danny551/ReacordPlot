#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot/qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitWidget();
    void InitCustomPlot();

private slots:
    void slot_DataTimeOut();
    void slot_CusTimeOut();
    void slot_SelectionChanged();

    void mousePress(QMouseEvent *mevent);
    void mouseMove(QMouseEvent *mevent);
    void mouseRelease(QMouseEvent *mevent);

    void on_action_start_triggered();
    void on_action_stop_triggered();
    void on_action_recovery_triggered();

private:
    Ui::MainWindow *ui;

    QCustomPlot *m_Multichannel;
    QCPItemTracer *tracer;
    QCPItemText *tracerLabel;
    QCPGraph *tracerGraph;
    QRubberBand *rubberBand;
    QPoint rubberOrigin;

    QTimer *dataTimer;
    QTimer *cusTimer;

    int xCount;
    bool chooseFlag;
};
#endif // MAINWINDOW_H
