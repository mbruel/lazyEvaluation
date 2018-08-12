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
#include "ComputationTask.h"
#include "Element.h"
#include <QThread>
#include <QDebug>
#include <QWaitCondition>

QAtomicUInt ComputationTask::sNbAbortedTasks = 0;

ComputationTask::ComputationTask(Element *element):
    _result(new Element(*element)), _isCancelled(0x0),
    _mutexTask(), _waitOnTask(), _isTaskManagerWaiting(false)
{
    setAutoDelete(true);
}

void ComputationTask::waitUntilTaskIsComplete()
{
    _isTaskManagerWaiting = true;
    _mutexTask.lock();
    _waitOnTask.wait(&_mutexTask);
    _mutexTask.unlock();
}

bool ComputationTask::preRun()
{
    if (_isCancelled.load() != 0x0)
    {
        qDebug() << "[ComputationTask::run] Task didn't run for Element: " << _result->str();
        delete _result;
        sNbAbortedTasks++;
        return false;
    }
    return true;
}

void ComputationTask::postRun()
{
    // So the EvolutionManager can fetch the result (we transfer its ownership)
    emit ComputationTaskComplete(this);

    // In case the EvolutionManager is waiting for a result
    if (_isTaskManagerWaiting)
        _waitOnTask.notify_one();
}

