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
#ifndef ELEMENT_H
#define ELEMENT_H

#include <QString>
#include <QAtomicInteger>

class LazyComputationManager;
using QAtomicBool = QAtomicInteger<unsigned short>; // 16 bit only (faster than using 8 bit variable...)

class Element {
    friend class EvolutionTask;

public:
    Element(float mass);
    Element(const Element &src);
    ~Element();

    Element clone();
    QString str() const;

    void computeEvolution(float timeFromNow);
    bool update();

    inline bool isInEvolution() const;
    inline void setInEvolution(bool enterInEvolution);

    inline double getMass() const;

private:
    ushort      _id;
    double      _mass;
    QAtomicBool _isInEvolution;


    static LazyComputationManager  *evolManager;
    static ushort                   nextId;
};

double Element::getMass() const { return _mass; }
bool Element::isInEvolution() const { return _isInEvolution.load() != 0x0; }

void Element::setInEvolution(bool isInEvolution)
{
    if (isInEvolution)
        _isInEvolution.testAndSetOrdered(0x0, 0x1);
    else
        _isInEvolution.testAndSetOrdered(0x1, 0x0);
}


#endif // ELEMENT_H
