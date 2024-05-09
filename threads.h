#pragma once

#include <unistd.h>
#include <pthread.h>
#include "mainwindow.h"

class MainWindow;
// void *cpuThread(MainWindow *mw);

void *cpuThread(MainWindow *mw)
{
    // MainWindow *mw = (MainWindow *)arg;
    while (true)
    {
        mw->cpu->update();
        mw->cpu_usage[0].push_front(mw->cpu->get_usage());
        if (mw->cpu_usage[0].size() > 61)
            mw->cpu_usage[0].pop_back();
        for (int i = 0; i < mw->cpu->get_processors_count(); i++)
        {
            mw->cpu_usage[i + 1].push_front(mw->cpu->get_usage(i));
            if (mw->cpu_usage[i + 1].size() > 61)
                mw->cpu_usage[0].pop_back();
        }
        mw->cpu_frequency[0].push_front(mw->cpu->get_freq());
        if (mw->cpu_frequency[0].size() > 61)
            mw->cpu_frequency[0].pop_back();
        for (int i = 0; i < mw->cpu->get_processors_count(); i++)
        {
            mw->cpu_frequency[i + 1].push_front(mw->cpu->get_freq(i));
            if (mw->cpu_frequency[i + 1].size() > 61)
                mw->cpu_frequency[0].pop_back();
        }
        mw->usage->graph(0)->setData(mw->time, mw->cpu_usage[0]);
        for (int i = 1; i <= mw->cpu->get_processors_count(); i++)
            mw->usage->graph(i)->setData(mw->time, mw->cpu_usage[i]);
        mw->frequency->graph(0)->setData(mw->time, mw->cpu_frequency[0]);
        for (int i = 1; i <= mw->cpu->get_processors_count(); i++)
            mw->frequency->graph(i)->setData(mw->time, mw->cpu_frequency[i]);
        sleep(1);
        pthread_testcancel();
    }
}