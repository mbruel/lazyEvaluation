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
#include "EvolutionTask.h"
#include "Element.h"

EvolutionTask::EvolutionTask(Element *elem, float timeFromNow)
    : ComputationTask(elem), _timeFromNow(timeFromNow)
{}

#include <QThread>
void EvolutionTask::run()
{
    if (!preRun())
        return;

    // Some calculation on the copy the Matter
    _result->_mass = _timeFromNow;
    QThread::sleep(2);

    postRun();
}
