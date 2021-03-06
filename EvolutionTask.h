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
#ifndef EVOLUTIONTASK_H
#define EVOLUTIONTASK_H

#include "ComputationTask.h"
class Element;

class EvolutionTask : public ComputationTask
{
public:
    EvolutionTask(Element *elem, float timeFromNow);
    ~EvolutionTask() = default;

    void _run() override;

private:
    float _timeFromNow;
};

#endif // EVOLUTIONTASK_H
