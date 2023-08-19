#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("RecordPlot");

    InitWidget();
    InitCustomPlot();
}

MainWindow::~MainWindow()
{
    delete m_Multichannel;
    delete ui;
}

void MainWindow::InitWidget()
{
    //初始化一些变量
    xCount = 0;
    chooseFlag = false;
    ui->action_start->setEnabled(true);
    ui->action_stop->setEnabled(false);

    //数据添加定时器
    dataTimer = new QTimer();
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(slot_DataTimeOut()));

    //界面更新定时器
    cusTimer = new QTimer();
    connect(cusTimer, SIGNAL(timeout()), this, SLOT(slot_CusTimeOut()));
}

void MainWindow::InitCustomPlot()
{
    m_Multichannel = new QCustomPlot();
    m_Multichannel = ui->widget;
    m_Multichannel->setSizePolicy(QSizePolicy::Policy::Expanding,
                                  QSizePolicy::Policy::Expanding);
    // m_Multichannel->axisRect()->setBackground(QBrush(Qt::black));
    // //设置背景颜色

    for (int i = 0; i < 3; i++) {
        m_Multichannel->addGraph(); //添加数据曲线
        // m_Multichannel->graph(i)->setPen(QPen(Qt::black));
        // //设置相同颜色的曲线
    }
    // QPen pen;
    // pen.setWidth(1);   //设置选中时的线宽
    // 建议宽度设为1,如果数据量很大,界面会卡顿 pen.setColor(Qt::blue);
    // m_Multichannel->graph(0)->setPen(pen);   //这种方式不如下面的方便,屏蔽
    m_Multichannel->graph(0)->setPen(QPen(Qt::black)); //设置曲线颜色
    m_Multichannel->graph(0)->setName("曲线一");       //设置曲线名称
    m_Multichannel->graph(1)->setPen(QPen(Qt::red));
    m_Multichannel->graph(1)->setName("曲线二");
    m_Multichannel->graph(2)->setPen(QPen(Qt::green));
    m_Multichannel->graph(2)->setName("曲线三");
    // m_Multichannel->graph(0)->setBrush(QBrush(QColor(0,0,255,20)));
    // //在点击对应曲线时 区域变色
    // m_Multichannel->graph(0)->setAntialiasedFill(true);   //设置填充
    // m_Multichannel->graph(0)->setVisible(false);   //设置可见性

    // x轴设置
    QSharedPointer<QCPAxisTickerFixed> intTicker_M(new QCPAxisTickerFixed);
    intTicker_M->setTickStep(1); //设置刻度之间的步长为1
    intTicker_M->setScaleStrategy(
        QCPAxisTickerFixed::ssMultiples); //设置缩放策略
    m_Multichannel->xAxis->setTicker(
        intTicker_M); //应用自定义整形ticker,防止使用放大功能时出现相同的x刻度值
    m_Multichannel->xAxis->ticker()->setTickCount(11); //刻度数量
    m_Multichannel->xAxis->setNumberFormat("f");       // x轴刻度值格式
    m_Multichannel->xAxis->setNumberPrecision(0);      //刻度值精度
    m_Multichannel->xAxis->setLabel("数量(n)");        //设置标签
    m_Multichannel->xAxis->setLabelFont(
        QFont(font().family(), 8)); //设置标签字体大小
    m_Multichannel->xAxis->setRange(0, 10, Qt::AlignLeft); //范围
    m_Multichannel->xAxis->setSubTickLength(0, 0);         //子刻度长度
    m_Multichannel->xAxis->setTickLength(10, 5);           //主刻度长度
    // y轴设置
    m_Multichannel->yAxis->setNumberFormat("f");
    m_Multichannel->yAxis->setNumberPrecision(2);
    m_Multichannel->yAxis->setLabel("距离(m)");
    m_Multichannel->yAxis->setLabelFont(QFont(font().family(), 8));
    m_Multichannel->yAxis->setRange(0, 5);
    m_Multichannel->yAxis->setTickLength(10, 5);

    m_Multichannel->setInteractions(QCP::iNone); //设置不与鼠标交互
    // m_Multichannel->setInteractions(QCP::iSelectPlottables |
    // QCP::iSelectLegend | QCP::iRangeDrag);
    // //设置鼠标交互,曲线及图例可点击,可拖动
    m_Multichannel->legend->setVisible(true); //设置图例可见
    m_Multichannel->legend->setBrush(QColor(255, 255, 255, 0)); //设置背景透明
    m_Multichannel->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignRight); //设置图例居右上

    //游标
    tracer = new QCPItemTracer(m_Multichannel); //生成游标
    m_Multichannel->setMouseTracking(
        true); //让游标自动随鼠标移动，若不想游标随鼠标动，则禁止
    // tracer->setPen(QPen(QBrush(QColor(Qt::red)),Qt::DashLine));   //虚线游标
    tracer->setPen(QPen(Qt::red));             //圆圈轮廓颜色
    tracer->setBrush(QBrush(Qt::red));         //圆圈圈内颜色
    tracer->setStyle(QCPItemTracer::tsCircle); //圆圈
    tracer->setSize(5);
    // tracer->setVisible(false);                      //设置可见性

    //游标说明
    tracerLabel = new QCPItemText(m_Multichannel); //生成游标说明
    // tracerLabel->setVisible(false); //设置可见性
    tracerLabel->setLayer("overlay"); //设置图层为overlay，因为需要频繁刷新
    tracerLabel->setPen(QPen(Qt::black)); //设置游标说明颜色
    tracerLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop); //左上
    tracerLabel->setFont(QFont(font().family(), 10)); //字体大小
    tracerLabel->setPadding(QMargins(4, 4, 4, 4)); //文字距离边框几个像素
    tracerLabel->position->setParentAnchor(
        tracer->position); //设置标签自动随着游标移动

    //选择不同的曲线
    connect(m_Multichannel, SIGNAL(selectionChangedByUser()), this,
            SLOT(slot_SelectionChanged()));

    //初始化QRubberBand   //矩形放大
    rubberBand = new QRubberBand(QRubberBand::Rectangle, m_Multichannel);
    //连接鼠标事件发出的信号，实现绑定
    connect(m_Multichannel, SIGNAL(mousePress(QMouseEvent *)), this,
            SLOT(mousePress(QMouseEvent *)));
    connect(m_Multichannel, SIGNAL(mouseMove(QMouseEvent *)), this,
            SLOT(mouseMove(QMouseEvent *)));
    connect(m_Multichannel, SIGNAL(mouseRelease(QMouseEvent *)), this,
            SLOT(mouseRelease(QMouseEvent *)));

    // lambda表达式 mouseMoveEvent
    connect(m_Multichannel, &QCustomPlot::mouseMove, [=](QMouseEvent *event) {
        if (tracer->graph() == nullptr) {
            return;
        }
        if (tracer->graph()->data()->isEmpty()) {
            return;
        }
        if (tracer->visible()) {
            if (tracerGraph) {
                double x =
                    m_Multichannel->xAxis->pixelToCoord(event->pos().x());
                tracer->setGraphKey(x); //将游标横坐标设置成刚获得的横坐标数据x
                // tracer->setInterpolating(true);
                // //自动计算y值,若只想看已有点,不需要这个
                tracer->updatePosition(); //使得刚设置游标的横纵坐标位置生效
                tracerLabel->setText(QString("x:%1\ny:%2")
                                         .arg(tracer->position->key())
                                         .arg(tracer->position->value()));
                m_Multichannel->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    });
}

//多通道界面曲线改变
void MainWindow::slot_SelectionChanged()
{
    for (int i = 0; i < 3; i++) {
        QCPGraph *graph = m_Multichannel->graph(i);
        if (graph == nullptr) {
            return;
        }
        QCPPlottableLegendItem *item =
            m_Multichannel->legend->itemWithPlottable(graph);
        if (item->selected() ||
            graph->selected()) //选中了哪条曲线或者曲线的图例
        {
            tracerGraph = graph;
            if (tracer != nullptr) {
                tracer->setGraph(tracerGraph);
            }
            item->setSelected(true);
            QPen pen;
            pen.setWidth(
                1); //设置选中时的线宽 建议宽度设为1,如果数据量很大,界面会卡顿
            pen.setColor(Qt::blue);
            graph->selectionDecorator()->setPen(pen);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

//添加数据定时器
void MainWindow::slot_DataTimeOut()
{
    m_Multichannel->graph(0)->addData(xCount, 1); //添加数据使用addData
    m_Multichannel->graph(1)->addData(xCount, 3);
    m_Multichannel->graph(2)->addData(xCount, 5);
    xCount++;
}

//界面更新定时器
void MainWindow::slot_CusTimeOut()
{
    m_Multichannel->xAxis->setRange(0, xCount > 10 ? xCount : 10,
                                    Qt::AlignLeft); // xCount代表你的数据量
    m_Multichannel->yAxis->setRange(
        0, 6); //如果需要y轴自适应数据 就需要获取数据的最小最大值依次填入
    m_Multichannel->replot(
        QCustomPlot::rpQueuedReplot); //括号内参数作用是避免重复绘制
}

//鼠标按下槽函数
void MainWindow::mousePress(QMouseEvent *mevent)
{
    if (chooseFlag) {
        if (mevent->button() == Qt::RightButton) //鼠标右键实现放大功能
        {
            rubberOrigin = mevent->pos();
            rubberBand->setGeometry(QRect(rubberOrigin, QSize()));
            rubberBand->show();
        }
    }
}

//鼠标移动槽函数
void MainWindow::mouseMove(QMouseEvent *mevent)
{
    if (chooseFlag) {
        if (rubberBand->isVisible()) {
            rubberBand->setGeometry(
                QRect(rubberOrigin, mevent->pos()).normalized());
        }
    }
}

//鼠标释放槽函数
void MainWindow::mouseRelease(QMouseEvent *mevent)
{
    if (chooseFlag) {
        Q_UNUSED(mevent);
        if (rubberBand->isVisible()) {
            const QRect zoomRect = rubberBand->geometry();
            int xp1, yp1, xp2, yp2;
            zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
            double x1 = m_Multichannel->xAxis->pixelToCoord(xp1);
            double x2 = m_Multichannel->xAxis->pixelToCoord(xp2);
            double y1 = m_Multichannel->yAxis->pixelToCoord(yp1);
            double y2 = m_Multichannel->yAxis->pixelToCoord(yp2);

            m_Multichannel->xAxis->setRange(x1, x2);
            m_Multichannel->yAxis->setRange(y1, y2);

            rubberBand->hide();
            m_Multichannel->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void MainWindow::on_action_start_triggered()
{
    //初始化
    xCount = 0;
    for (int i = 0; i < 3; i++) {
        m_Multichannel->graph(i)->data().data()->clear(); //清除当前曲线数据
    }
    m_Multichannel->xAxis->setRange(0, 10, Qt::AlignLeft);
    m_Multichannel->yAxis->setRange(0, 5);
    m_Multichannel->replot(QCustomPlot::rpQueuedReplot);

    chooseFlag = false;
    dataTimer->start(100); //设置取数频率
    cusTimer->start(1000); //设置更新界面频率
    m_Multichannel->setInteractions(
        QCP::iSelectPlottables |
        QCP::iSelectLegend); //设置鼠标交互,曲线及图例可点击

    ui->action_start->setEnabled(false);
    ui->action_stop->setEnabled(true);
}

void MainWindow::on_action_stop_triggered()
{
    chooseFlag = true; //该标志代表界面更新定时器停止后,可以对界面进行放大操作
    dataTimer->stop();
    cusTimer->stop();
    m_Multichannel->setInteractions(
        QCP::iSelectPlottables | QCP::iSelectLegend |
        QCP::iRangeDrag); //设置鼠标交互,曲线及图例可点击,可拖动

    ui->action_start->setEnabled(true);
    ui->action_stop->setEnabled(false);
}

void MainWindow::on_action_recovery_triggered()
{
    if (chooseFlag) {
        m_Multichannel->xAxis->setRange(0, xCount > 10 ? xCount : 10,
                                        Qt::AlignLeft);
        m_Multichannel->yAxis->setRange(0, 6);
        m_Multichannel->replot(QCustomPlot::rpQueuedReplot);
    }
}
