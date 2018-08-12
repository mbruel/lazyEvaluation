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
#include "Element.h"
#include "LazyComputationManager.h"
#include <QDebug>
#include "EvolutionTask.h"

ushort                  Element::nextId      = 0;
LazyComputationManager *Element::evolManager = LazyComputationManager::getInstance();

Element::Element(float mass) :
    _id(nextId++),_mass(mass), _isInEvolution(0x0)
{}

Element::Element(const Element &src) :
    _id(src._id), _mass(src._mass), _isInEvolution(0x0)
{}

Element::~Element()
{
    qDebug() << "[Element] destroying elem " << _id;
    if (isInEvolution())
        evolManager->cancelTask(this);
}

Element Element::clone()
{
    Element elem(*this);
    elem._id = nextId++;
    if (update())
        elem._mass = _mass;
    return elem;
}

QString Element::str() const
{
    return QString("#%1 <mass %2> <isInEvolution: %3>").arg(_id).arg(_mass).arg(static_cast<int>(_isInEvolution));
}


void Element::computeEvolution(float timeFromNow)
{
    if (isInEvolution())
        update();

    setInEvolution(true);
    evolManager->computeTask(this, new EvolutionTask(this, timeFromNow));
}

bool Element::update()
{
    if (!isInEvolution())
        return false;

    Element *futurElem = evolManager->getResult(this);
    if (futurElem)
    {
        _mass = futurElem->getMass();
        delete futurElem; // we've the ownership of the Futur element
    }
    setInEvolution(false);
    return true;
}


