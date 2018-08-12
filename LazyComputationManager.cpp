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
#include "LazyComputationManager.h"
#include "Element.h"
#include "ComputationTask.h"
#include <QDebug>


LazyComputationManager::LazyComputationManager(ushort maxThread):
    _activeTasks(), _results(), _secureActiveTaskMap(), _secureResultMap(),
    _threadPool(), _maxThreads(maxThread),
    _nbScheduledTasks(0), _nbCancelledTasks(0),
    _maxThreadUsed(0), _avgThreadUsed(0)
{
    setMaxThread(maxThread);
}

LazyComputationManager::~LazyComputationManager()
{
    clear();
    _threadPool.waitForDone();
}

void LazyComputationManager::clear()
{
    QWriteLocker lockMap1(&_secureActiveTaskMap);
    QWriteLocker lockMap2(&_secureResultMap);

    // delete all the results that havn't been fetch by a element
    for (auto it = _results.cbegin(), itEnd = _results.cend(); it != itEnd ; ++it)
        delete it.value();

    // clear the maps
    _results.clear();
    _activeTasks.clear();

    _nbScheduledTasks = 0;
    _nbCancelledTasks = 0;
    _maxThreadUsed    = 0;
    _avgThreadUsed    = 0;
}

void LazyComputationManager::setMaxThread(ushort maxThread)
{
    _maxThreads = maxThread;
    _threadPool.setMaxThreadCount(maxThread);
}

void LazyComputationManager::computeTask(Element *element, ComputationTask *computationTask)
{
    QWriteLocker lockActiveTaskMap(&_secureActiveTaskMap);
    ++_nbScheduledTasks;
    connect(computationTask, &ComputationTask::ComputationTaskComplete, this, &LazyComputationManager::handleTaskCompletion, Qt::DirectConnection);
    _activeTasks.insert(element, computationTask);
    _threadPool.start(computationTask);

    ushort nbRunningTasks = _threadPool.activeThreadCount();
    if (nbRunningTasks > _maxThreadUsed)
        _maxThreadUsed = nbRunningTasks;

    _avgThreadUsed = (_avgThreadUsed * (_nbScheduledTasks - 1) + nbRunningTasks) / _nbScheduledTasks;

    qDebug() << "[LazyComputationManager::computeTask] starting evolution Task for " << element->str();
}

Element *LazyComputationManager::getResult(Element *element)
{
    _secureActiveTaskMap.lockForRead();
    ComputationTask *computationTask = _activeTasks.value(element, nullptr);
    _secureActiveTaskMap.unlock();
    if (computationTask)
    {
        qDebug() << "[LazyComputationManager::getResult] waiting for the result for Element: " << element->str();

        // Wait for the task to finish on its on Mutex
        computationTask->waitUntilTaskIsComplete();
    }

    QWriteLocker lockResultMap(&_secureResultMap);
    Element *evolElement = _results.value(element, nullptr);
    if (evolElement)
    {
        _results.remove(element);
        return evolElement;
    }
    else
    {
        qCritical() << "[LazyComputationManager::getResult] Couldn't get the result for " << element->str();
    }
    return nullptr;
}

void LazyComputationManager::cancelTask(Element *element)
{
    _secureActiveTaskMap.lockForWrite();
    ++_nbCancelledTasks;
    auto it = _activeTasks.find(element);
    if (it != _activeTasks.end())
    {
        ComputationTask *computationTask = it.value();
        computationTask->cancelIfNotRunning();
        qDebug() << "[LazyComputationManager::handleTaskCompletion] evolution task cancelled for " << element->str();
        _activeTasks.erase(it);
        _secureActiveTaskMap.unlock();
    }
    else
    {
        _secureActiveTaskMap.unlock();
        QWriteLocker lockResultMap(&_secureResultMap);
        auto it = _results.find(element);
        if (it != _results.end())
        {
            Element *res = it.value();
            _results.erase(it);
            delete res;
        }
    }
}



void LazyComputationManager::handleTaskCompletion(ComputationTask *computationTask)
{
    Element *elementSrc = getTaskCaller(computationTask);
    if (elementSrc)
    {
        QWriteLocker lockActiveTaskMap(&_secureActiveTaskMap);
        _activeTasks.remove(elementSrc);

        QWriteLocker lockResultMap(&_secureResultMap);
        _results.insert(elementSrc, computationTask->getComputedElement());
        qDebug() << "[LazyComputationManager::handleTaskCompletion] evolution result is ready for Element: " << elementSrc->str();
    }
    else
    {
        qDebug() << "[LazyComputationManager::handleTaskCompletion] can't find the Element source of a Task...";
        delete computationTask->_result; // delete temporary allocated Element evolved
    }
}

Element *LazyComputationManager::getTaskCaller(ComputationTask *computationTask)
{
    QReadLocker lockActiveTaskMap(&_secureActiveTaskMap);
    for (auto it = _activeTasks.cbegin() ; it != _activeTasks.cend(); ++it)
    {
        if (it.value() == computationTask)
            return it.key();
    }
    return nullptr;
}



void LazyComputationManager::dumpStatistics(bool waitEndTasks)
{
    if (waitEndTasks)
        _threadPool.waitForDone();

    qDebug() << "[LazyComputationManager::dumpStatistics]"
             << "\n\t - ThreadPool size: " << _maxThreads
             << "\n\t - Max Number of threads : " << _maxThreadUsed
             << "\n\t - AVG Number of threads : " << _avgThreadUsed
             << "\n\t - Nb Scheduled Tasks: " << getNbScheduledTasks()
             << "\n\t - Nb Cancelled Tasks: " << getNbCancelledTasks()
             << "\n\t - Nb Aborted Tasks:" << ComputationTask::getNbAbortedTasks();
}
