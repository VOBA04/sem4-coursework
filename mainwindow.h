#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "Cpu.h"
#include "Memory.h"
#include "Disk.h"
#include "Net.h"
#include <pthread.h>
#include "percentticker.h"
// #include "threads.h"

class MainWindow : public QMainWindow, protected Ui::MainWindow
{
    Q_OBJECT

    Cpu *cpu;
    Memory *memory;
    Disk *disks;
    Net *nets;
    QList<CustomPlot *> disk_graphs;
    CustomPlot *ethernet, *wireless;

    QVector<double> time;

    QVector<QVector<double>> cpu_usage;
    QVector<QVector<double>> cpu_frequency;
    QVector<double> memory_usage;
    QVector<double> memory_swap;
    QVector<QVector<double>> disks_read_speed;
    QVector<QVector<double>> disks_write_speed;
    QVector<double> ethernet_receiving_speed;
    QVector<double> ethernet_transmiting_speed;
    QVector<double> wireless_receiving_speed;
    QVector<double> wireless_transmiting_speed;

    pthread_t cpu_thread;
    pthread_t memory_thread;
    pthread_t disks_thread;
    pthread_t nets_thread;

    void setButtons();
    void setGraphs();
    void setVectors();
    void setThreads();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static void *cpuThread(void *);
    static void *memoryThread(void *);
    static void *disksThread(void *);
    static void *netsThread(void *);

public slots:
    void openCurrentAndCloseOtherGraphs(QListWidgetItem *);
};
#endif // MAINWINDOW_H
