#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    cpu = new Cpu();
    memory = new Memory();
    disks = new Disk();
    nets = new Net();
    setButtons();
    setGraphs();
    setVectors();
    openCurrentAndCloseOtherGraphs(buttons->item(0));
    setThreads();
}

MainWindow::~MainWindow()
{
    pthread_cancel(cpu_thread);
    pthread_join(cpu_thread, NULL);
    pthread_cancel(memory_thread);
    pthread_join(memory_thread, NULL);
    pthread_cancel(disks_thread);
    pthread_join(disks_thread, NULL);
    if (nets->get_interface(ETHERNET) || nets->get_interface(WIRELESS))
    {
        pthread_cancel(nets_thread);
        pthread_join(nets_thread, NULL);
    }
}

void MainWindow::setButtons()
{
    buttons->addItem(new QListWidgetItem(tr("CPU")));
    buttons->addItem(new QListWidgetItem(tr("Memory")));
    for (int i = 0; i < disks->get_disks_count(); i++)
        buttons->addItem(new QListWidgetItem(tr("Disk ") + QVariant(i + 1).toString()));
    if (nets->get_interface(ETHERNET))
        buttons->addItem(new QListWidgetItem("Ethernet"));
    if (nets->get_interface(WIRELESS))
        buttons->addItem(new QListWidgetItem(tr("Wireless Network")));
    for (int i = 0; i < buttons->count(); i++)
    {
        QListWidgetItem *button = buttons->item(i);
        button->setSizeHint(QSize(140, 50));
        button->setTextAlignment(Qt::AlignCenter);
        QFont font = button->font();
        font.setPointSize(14);
        button->setFont(font);
        buttons->setWordWrap(true);
    }
    connect(buttons, &QListWidget::itemActivated,
            this, &MainWindow::openCurrentAndCloseOtherGraphs);
}

void MainWindow::setGraphs()
{
    int step;
    QVector<QPair<QString, QString>> usage_legend_columns, freq_legend_columns;
    usage_legend_columns.append(QPair<QString, QString>("CPU", "%"));
    freq_legend_columns.append(QPair<QString, QString>("CPU", tr("MHz")));
    for (int i = 0; i < cpu->get_processors_count(); i++)
    {
        usage_legend_columns.append(QPair<QString, QString>("CPU" + QString::number(i), "%"));
        freq_legend_columns.append(QPair<QString, QString>("CPU" + QString::number(i), tr("MHz")));
    }
    usage->yAxis2->setVisible(true);
    usage->yAxis2->setTickLabels(true);
    usage->yAxis2->setTicks(true);
    usage->xAxis2->setVisible(true);
    usage->xAxis2->setTicks(false);
    usage->yAxis->setTicks(false);
    usage->setRangeWithTicker(usage->xAxis, 0, 60, 10, tr("s"));
    usage->setRangeWithTicker(usage->yAxis2, 0, 100, 20, "%");
    usage->yAxis->setRange(0, 100);
    usage->xAxis->setRangeReversed(true);
    usage->addGraph();
    usage->graph(0)->setPen(QPen(Qt::red));
    usage->graph(0)->setName("cpu");
    step = 360 / (cpu->get_processors_count() + 2);
    for (int i = 1; i <= cpu->get_processors_count(); i++)
    {
        usage->addGraph();
        usage->graph(i)->setPen(QPen(QColor::fromHsv(step * i, 255, 255)));
        usage->graph(i)->setName("cpu" + QVariant(i - 1).toString());
    }
    usage->setLegend(usage_legend_columns, cpu->get_processors_count() / 2 + 1);
    usage->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(usage, QString::fromStdString(cpu->get_model()), QFont("sans", 14, QFont::Bold));
    usage->plotLayout()->addElement(0, 0, title);

    frequency->yAxis2->setVisible(true);
    frequency->yAxis2->setTickLabels(true);
    frequency->yAxis2->setTicks(true);
    frequency->xAxis2->setVisible(true);
    frequency->xAxis2->setTicks(false);
    frequency->yAxis->setTicks(false);
    frequency->setRangeWithTicker(frequency->yAxis2, 0,
                                  qMin(cpu->get_max_freq() * 2 - (cpu->get_max_freq() * 2) % 500, 8000), 500, tr("MHz"));
    frequency->yAxis->setRange(0, qMin(cpu->get_max_freq() * 2 - (cpu->get_max_freq() * 2) % 500, 8000));
    frequency->setRangeWithTicker(frequency->xAxis, 0, 60, 10, tr("s"));
    frequency->xAxis->setRangeReversed(true);
    frequency->addGraph();
    frequency->graph(0)->setPen(QPen(Qt::red));
    frequency->graph(0)->setName("cpu");
    step = 360 / (cpu->get_processors_count() + 2);
    for (int i = 1; i <= cpu->get_processors_count(); i++)
    {
        frequency->addGraph();
        frequency->graph(i)->setPen(QPen(QColor::fromHsv(step * i, 255, 255)));
        frequency->graph(i)->setName("cpu" + QVariant(i - 1).toString());
    }
    frequency->setLegend(freq_legend_columns);

    QVector<QPair<QString, QString>> memory_legend_columns;
    memory_legend_columns.append(QPair<QString, QString>("Memory", "%"));
    memory_legend_columns.append(QPair<QString, QString>("Swap", "%"));
    memory_graph->yAxis2->setVisible(true);
    memory_graph->yAxis2->setTickLabels(true);
    memory_graph->yAxis2->setTicks(true);
    memory_graph->xAxis2->setVisible(true);
    memory_graph->xAxis2->setTicks(false);
    memory_graph->yAxis->setTicks(false);
    memory_graph->yAxis->setRange(0, 100);
    memory_graph->setRangeWithTicker(memory_graph->yAxis2, 0, 100, 20, "%");
    memory_graph->setRangeWithTicker(memory_graph->xAxis, 0, 60, 10, tr("s"));
    memory_graph->xAxis->setRangeReversed(true);
    memory_graph->addGraph();
    memory_graph->graph(0)->setPen(QPen(Qt::red));
    memory_graph->graph(0)->setName(tr("Memory"));
    memory_graph->addGraph();
    memory_graph->graph(1)->setPen(QPen(Qt::green));
    memory_graph->graph(1)->setName(tr("Swap"));
    memory_graph->setLegend(memory_legend_columns);

    for (int i = 0; i < disks->get_disks_count(); i++)
    {
        QVector<QPair<QString, QString>> disk_legend_columns;
        disk_legend_columns.append(QPair<QString, QString>("Reading", tr("MB/s")));
        disk_legend_columns.append(QPair<QString, QString>("Writing", tr("MB/s")));
        disk_graphs.append(new CustomPlot(centralwidget));
        disk_graphs[i]->setObjectName("disk " + QVariant(i).toString());
        graphs->addWidget(disk_graphs[i]);
        disk_graphs[i]->yAxis2->setVisible(true);
        disk_graphs[i]->yAxis2->setTickLabels(true);
        disk_graphs[i]->yAxis2->setTicks(true);
        disk_graphs[i]->xAxis2->setVisible(true);
        disk_graphs[i]->xAxis2->setTicks(false);
        disk_graphs[i]->yAxis->setTicks(false);
        disk_graphs[i]->yAxis->setRange(0, 20);
        disk_graphs[i]->setRangeWithTicker(disk_graphs[i]->xAxis, 0, 60, 10, tr("s"));
        disk_graphs[i]->setRangeWithTicker(disk_graphs[i]->yAxis2, 0, 20, 2, tr("MB/s"));
        disk_graphs[i]->xAxis->setRangeReversed(true);
        disk_graphs[i]->addGraph();
        disk_graphs[i]->graph(0)->setPen(QPen(Qt::red));
        disk_graphs[i]->graph(0)->setName(tr("Reading"));
        disk_graphs[i]->addGraph();
        disk_graphs[i]->graph(1)->setPen(QPen(Qt::blue));
        disk_graphs[i]->graph(1)->setName(tr("Writing"));
        disk_graphs[i]->setLegend(disk_legend_columns);
        disk_graphs[i]->plotLayout()->insertRow(0);
        QCPTextElement *title = new QCPTextElement(disk_graphs[i], QString::fromStdString(disks->get_model(i)), QFont("sans", 14, QFont::Bold));
        disk_graphs[i]->plotLayout()->addElement(0, 0, title);
    }

    if (nets->get_interface(ETHERNET))
    {
        QVector<QPair<QString, QString>> ethernet_legend_columns;
        ethernet_legend_columns.append(QPair<QString, QString>("Receiving", tr("Mb/s")));
        ethernet_legend_columns.append(QPair<QString, QString>("Transmiting", tr("Mb/s")));
        ethernet = new CustomPlot(centralwidget);
        ethernet->setObjectName("ethernet");
        graphs->addWidget(ethernet);
        ethernet->yAxis2->setVisible(true);
        ethernet->yAxis2->setTickLabels(true);
        ethernet->yAxis2->setTicks(true);
        ethernet->xAxis2->setVisible(true);
        ethernet->xAxis2->setTicks(false);
        ethernet->yAxis->setTicks(false);
        ethernet->yAxis->setRange(0, 10);
        ethernet->setRangeWithTicker(ethernet->xAxis, 0, 60, 10, tr("s"));
        ethernet->setRangeWithTicker(ethernet->yAxis2, 0, 10, 1, tr("Mb/s"));
        ethernet->xAxis->setRangeReversed(true);
        ethernet->addGraph();
        ethernet->graph(0)->setPen(QPen(Qt::red));
        ethernet->graph(0)->setName(tr("Receiving"));
        ethernet->addGraph();
        ethernet->graph(1)->setPen(QPen(Qt::blue));
        ethernet->graph(1)->setName(tr("Transmiting"));
        ethernet->setLegend(ethernet_legend_columns);
    }
    if (nets->get_interface(WIRELESS))
    {
        QVector<QPair<QString, QString>> wireless_legend_columns;
        wireless_legend_columns.append(QPair<QString, QString>("Receiving", tr("Mb/s")));
        wireless_legend_columns.append(QPair<QString, QString>("Transmiting", tr("Mb/s")));
        wireless = new CustomPlot(centralwidget);
        wireless->setObjectName("wireless");
        graphs->addWidget(wireless);
        wireless->yAxis2->setVisible(true);
        wireless->yAxis2->setTickLabels(true);
        wireless->yAxis2->setTicks(true);
        wireless->xAxis2->setVisible(true);
        wireless->xAxis2->setTicks(false);
        wireless->yAxis->setTicks(false);
        wireless->yAxis->setRange(0, 10);
        wireless->setRangeWithTicker(wireless->xAxis, 0, 60, 10, tr("s"));
        wireless->setRangeWithTicker(wireless->yAxis2, 0, 10, 1, tr("Mb/s"));
        wireless->xAxis->setRangeReversed(true);
        wireless->addGraph();
        wireless->graph(0)->setPen(QPen(Qt::red));
        wireless->graph(0)->setName(tr("Receiving"));
        wireless->addGraph();
        wireless->graph(1)->setPen(QPen(Qt::blue));
        wireless->graph(1)->setName(tr("Transmiting"));
        wireless->setLegend(wireless_legend_columns);
    }
}

void MainWindow::setVectors()
{
    for (float i = 0.0; i <= 60.0; i += UPD_TIME)
    {
        time.append(i);
        memory_usage.append(0);
        memory_swap.append(0);
        ethernet_receiving_speed.append(0);
        ethernet_transmiting_speed.append(0);
        wireless_receiving_speed.append(0);
        wireless_transmiting_speed.append(0);
    }
    cpu_usage = QVector<QVector<double>>(cpu->get_processors_count() + 1);
    cpu_frequency = QVector<QVector<double>>(cpu->get_processors_count() + 1);
    for (int i = 0; i <= cpu->get_processors_count(); i++)
        for (float j = 0.0; j <= 60.0; j += UPD_TIME)
        {
            cpu_usage[i].append(0);
            cpu_frequency[i].append(0);
        }
    disks_read_speed = QVector<QVector<double>>(disks->get_disks_count());
    disks_write_speed = QVector<QVector<double>>(disks->get_disks_count());
    for (int i = 0; i < disks->get_disks_count(); i++)
        for (float j = 0.0; j <= 60.0; j += UPD_TIME)
        {
            disks_read_speed[i].append(0);
            disks_write_speed[i].append(0);
        }
}

void MainWindow::setThreads()
{
    pthread_create(&cpu_thread, NULL, cpuThread, this);
    pthread_create(&memory_thread, NULL, memoryThread, this);
    pthread_create(&disks_thread, NULL, disksThread, this);
    if (nets->get_interface(ETHERNET) || nets->get_interface(WIRELESS))
    {
        pthread_create(&nets_thread, NULL, netsThread, this);
    }
}

void *MainWindow::cpuThread(void *arg)
{
    MainWindow *mw = (MainWindow *)arg;
    while (true)
    {
        mw->cpu->update();
        mw->cpu_usage[0].push_front(mw->cpu->get_usage());
        mw->cpu_usage[0].pop_back();
        for (int i = 0; i < mw->cpu->get_processors_count(); i++)
        {
            mw->cpu_usage[i + 1].push_front(mw->cpu->get_usage(i));
            mw->cpu_usage[i + 1].pop_back();
        }
        mw->cpu_frequency[0].push_front(mw->cpu->get_freq());
        mw->cpu_frequency[0].pop_back();
        for (int i = 0; i < mw->cpu->get_processors_count(); i++)
        {
            mw->cpu_frequency[i + 1].push_front(mw->cpu->get_freq(i));
            mw->cpu_frequency[i + 1].pop_back();
        }
        mw->usage->graph(0)->setData(mw->time, mw->cpu_usage[0]);
        ((SquareLegendItem *)mw->usage->legend->item(0))
            ->setVal(QString::number(mw->cpu_usage[0][0], 'f', 2) + "%");
        for (int i = 1; i <= mw->cpu->get_processors_count(); i++)
        {
            mw->usage->graph(i)->setData(mw->time, mw->cpu_usage[i]);
            ((SquareLegendItem *)mw->usage->legend->item(i))
                ->setVal(QString::number(mw->cpu_usage[i][0], 'f', 2) + "%");
        }
        mw->usage->replot();
        mw->frequency->graph(0)->setData(mw->time, mw->cpu_frequency[0]);
        ((SquareLegendItem *)mw->frequency->legend->item(0))
            ->setVal(QString::number(mw->cpu_frequency[0][0], 'f', 0) + tr("MHz"));
        for (int i = 1; i <= mw->cpu->get_processors_count(); i++)
        {
            mw->frequency->graph(i)->setData(mw->time, mw->cpu_frequency[i]);
            ((SquareLegendItem *)mw->frequency->legend->item(i))
                ->setVal(QString::number(mw->cpu_frequency[i][0], 'f', 0) + tr("MHz"));
        }
        mw->frequency->replot();
        usleep(UPD_TIME * 1000000);
        pthread_testcancel();
    }
}

void *MainWindow::memoryThread(void *arg)
{
    MainWindow *mw = (MainWindow *)arg;
    while (true)
    {
        mw->memory->update();
        mw->memory_usage.push_front((mw->memory->get_total() - mw->memory->get_available()) /
                                    mw->memory->get_total() * 100.0);
        mw->memory_usage.pop_back();
        mw->memory_swap.push_front((mw->memory->get_swap_total() - mw->memory->get_swap_free()) /
                                   mw->memory->get_swap_total() * 100.0);
        mw->memory_swap.pop_back();
        mw->memory_graph->graph(0)->setData(mw->time, mw->memory_usage);
        mw->memory_graph->graph(1)->setData(mw->time, mw->memory_swap);
        ((SquareLegendItem *)mw->memory_graph->legend->item(0))
            ->setVal(QString::number(mw->memory_usage[0], 'f', 2) + "% " +
                     QString::number(mw->memory->get_total() - mw->memory->get_available(), 'f', 1) + tr("GB of ") +
                     QString::number(mw->memory->get_total(), 'f', 1) + tr("GB"));
        ((SquareLegendItem *)mw->memory_graph->legend->item(1))
            ->setVal(QString::number(mw->memory_swap[0], 'f', 2) + "% " +
                     QString::number(mw->memory->get_swap_total() - mw->memory->get_swap_free(), 'f', 1) + tr("GB of ") +
                     QString::number(mw->memory->get_swap_total(), 'f', 1) + tr("GB"));
        mw->memory_graph->replot();
        usleep(UPD_TIME * 1000000);
        pthread_testcancel();
    }
}

void *MainWindow::disksThread(void *arg)
{
    MainWindow *mw = (MainWindow *)arg;
    while (true)
    {
        mw->disks->update();
        for (int i = 0; i < mw->disks->get_disks_count(); i++)
        {
            mw->disks_read_speed[i].push_front(mw->disks->get_read_speed(i));
            mw->disks_read_speed[i].pop_back();
            mw->disks_write_speed[i].push_front(mw->disks->get_write_speed(i));
            mw->disks_write_speed[i].pop_back();
            mw->disk_graphs[i]->graph(0)->setData(mw->time, mw->disks_read_speed[i]);
            mw->disk_graphs[i]->graph(1)->setData(mw->time, mw->disks_write_speed[i]);
            ((SquareLegendItem *)mw->disk_graphs[i]->legend->item(0))
                ->setVal(QString::number(mw->disks_read_speed[i][0], 'f', 2) + tr("MB/s"));
            ((SquareLegendItem *)mw->disk_graphs[i]->legend->item(1))
                ->setVal(QString::number(mw->disks_write_speed[i][0], 'f', 2) + tr("MB/s"));
            mw->disk_graphs[i]->replot();
        }
        sleep(1);
        pthread_testcancel();
    }
}

void *MainWindow::netsThread(void *arg)
{
    MainWindow *mw = (MainWindow *)arg;
    while (true)
    {
        mw->nets->update();
        if (mw->nets->get_interface(ETHERNET))
        {
            mw->ethernet_receiving_speed.push_front(mw->nets->get_receiving_speed(ETHERNET));
            mw->ethernet_receiving_speed.pop_back();
            mw->ethernet_transmiting_speed.push_front(mw->nets->get_transmiting_speed(ETHERNET));
            mw->ethernet_transmiting_speed.pop_back();
            mw->ethernet->graph(0)->setData(mw->time, mw->ethernet_receiving_speed);
            mw->ethernet->graph(1)->setData(mw->time, mw->ethernet_transmiting_speed);
            ((SquareLegendItem *)mw->ethernet->legend->item(0))
                ->setVal(QString::number(mw->ethernet_receiving_speed[0], 'f', 2) + tr("Mb/s"));
            ((SquareLegendItem *)mw->ethernet->legend->item(1))
                ->setVal(QString::number(mw->ethernet_transmiting_speed[0], 'f', 2) + tr("Mb/s"));
            mw->ethernet->replot();
        }
        if (mw->nets->get_interface(WIRELESS))
        {
            mw->wireless_receiving_speed.push_front(mw->nets->get_receiving_speed(WIRELESS));
            mw->wireless_receiving_speed.pop_back();
            mw->wireless_transmiting_speed.push_front(mw->nets->get_transmiting_speed(WIRELESS));
            mw->wireless_transmiting_speed.pop_back();
            mw->wireless->graph(0)->setData(mw->time, mw->wireless_receiving_speed);
            mw->wireless->graph(1)->setData(mw->time, mw->wireless_transmiting_speed);
            ((SquareLegendItem *)mw->wireless->legend->item(0))
                ->setVal(QString::number(mw->wireless_receiving_speed[0], 'f', 2) + tr("Mb/s"));
            ((SquareLegendItem *)mw->wireless->legend->item(1))
                ->setVal(QString::number(mw->wireless_transmiting_speed[0], 'f', 2) + tr("Mb/s"));
            mw->wireless->replot();
        }
        sleep(1);
        pthread_testcancel();
    }
}

void MainWindow::openCurrentAndCloseOtherGraphs(QListWidgetItem *item)
{
    QString name = item->text();
    if (name == "CPU")
    {
        memory_window->hide();
        for (int i = 0; i < disks->get_disks_count(); i++)
            disk_graphs[i]->hide();
        if (nets->get_interface(ETHERNET))
            ethernet->hide();
        if (nets->get_interface(WIRELESS))
            wireless->hide();
        cpu_graphs->show();
        return;
    }
    if (name == "Memory")
    {
        cpu_graphs->hide();
        for (int i = 0; i < disks->get_disks_count(); i++)
            disk_graphs[i]->hide();
        if (nets->get_interface(ETHERNET))
            ethernet->hide();
        if (nets->get_interface(WIRELESS))
            wireless->hide();
        memory_window->show();
        return;
    }
    if (name.contains("Disk"))
    {
        cpu_graphs->hide();
        memory_window->hide();
        for (int i = 0; i < disks->get_disks_count(); i++)
            disk_graphs[i]->hide();
        if (nets->get_interface(ETHERNET))
            ethernet->hide();
        if (nets->get_interface(WIRELESS))
            wireless->hide();
        int number = name.sliced(5).toInt();
        disk_graphs[number - 1]->show();
    }
    if (name == "Ethernet")
    {
        cpu_graphs->hide();
        memory_window->hide();
        for (int i = 0; i < disks->get_disks_count(); i++)
            disk_graphs[i]->hide();
        if (nets->get_interface(WIRELESS))
            wireless->hide();
        ethernet->show();
    }
    if (name == "Wireless Network")
    {
        cpu_graphs->hide();
        memory_window->hide();
        for (int i = 0; i < disks->get_disks_count(); i++)
            disk_graphs[i]->hide();
        if (nets->get_interface(ETHERNET))
            ethernet->hide();
        wireless->show();
    }
}