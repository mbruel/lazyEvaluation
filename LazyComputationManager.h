//==================================================================================
//
// Copyright (C) 2018 Matthieu Bruel <Matthieu.Bruel@gmail.com>
//
// This file is a part of lazyEvaluation : https://github.com/mbruel/lazyEvaluation
//
// lazyEvaluation is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 3.0 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301,
// USA.
//
//==================================================================================
#ifndef LAZYCOMPUTATIONMANAGER_H
#define LAZYCOMPUTATIONMANAGER_H

#include "Singleton.h"

class Element;
class ComputationTask;

#include <QThreadPool>
#include <QMap>
#include <QSet>
#include <QReadWriteLock>

// QObject so it can have a slot that receive the ending of a finished task
// in order to delete it if it is Orphan
class LazyComputationManager : public QObject, public Singleton<LazyComputationManager>
{
     Q_OBJECT
    friend class Singleton<LazyComputationManager>;

public:
    LazyComputationManager(ushort maxThread = QThread::idealThreadCount());
    ~LazyComputationManager();

    void setMaxThread(ushort maxThread);

    void     computeTask(Element *Element, ComputationTask *computationTask);
    Element  *getResult(Element *element);
    void     cancelTask(Element *element);

    void clear();
    inline void waitForDone();

    inline ulong getNbScheduledTasks() const;
    inline ulong getNbCancelledTasks() const;

    void dumpStatistics(bool waitEndTasks = true);

public slots:
    void handleTaskCompletion(ComputationTask *computationTask);

private:
    Element *getTaskCaller(ComputationTask *computationTask);

private:
    QMap<Element *, ComputationTask *> _activeTasks;
    QMap<Element *, Element *>         _results;
    QReadWriteLock                     _secureActiveTaskMap;
    QReadWriteLock                     _secureResultMap;
    QThreadPool                        _threadPool;
    ushort                             _maxThreads;

    ulong                              _nbScheduledTasks;
    ulong                              _nbCancelledTasks;
    ushort                             _maxThreadUsed;
    float                              _avgThreadUsed;
};

void  LazyComputationManager::waitForDone() { _threadPool.waitForDone(); }
ulong LazyComputationManager::getNbScheduledTasks() const { return _nbScheduledTasks; }
ulong LazyComputationManager::getNbCancelledTasks() const { return _nbCancelledTasks; }


#endif // LAZYCOMPUTATIONMANAGER_H
