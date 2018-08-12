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
#ifndef COMPUTATIONTASK_H
#define COMPUTATIONTASK_H

#include <QRunnable>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class Element;
class EvolutionManager;

using QAtomicBool = QAtomicInteger<unsigned short>; // 16 bit only (faster than using 8 bit variable...)
using QAtomicUInt = QAtomicInteger<unsigned int>; // 32 bits

// Inherit from QObject to be able to signal when it is done
class ComputationTask : public QObject, public QRunnable
{
    Q_OBJECT
    friend  class LazyComputationManager;

public:
    virtual ~ComputationTask() = default;

    virtual void run() = 0;
    inline Element *getComputedElement();

    bool preRun();
    void postRun();


    inline void cancelIfNotRunning();

    static inline unsigned int getNbAbortedTasks();

signals:
    void ComputationTaskComplete(ComputationTask *evolTask);

protected:
    ComputationTask(Element *element);

private:
    void waitUntilTaskIsComplete(); // used by the LazyComputationManager

protected:
    Element       *_result;

private:
    QAtomicBool   _isCancelled;

    QMutex         _mutexTask;
    QWaitCondition _waitOnTask;
    bool           _isTaskManagerWaiting;

    static QAtomicUInt sNbAbortedTasks;
};

Element *ComputationTask::getComputedElement(){ return _result; }
void ComputationTask::cancelIfNotRunning(){ _isCancelled.testAndSetOrdered(0x0, 0x1); }
unsigned int ComputationTask::getNbAbortedTasks() { return static_cast<int>(sNbAbortedTasks); }


#endif // COMPUTATIONTASK_H
