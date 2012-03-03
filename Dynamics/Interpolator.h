/*******************************************************************************
 Interpolator: Base class for interpolators.
 Copyright (c) 2006-2008

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <iostream>

#include "Integrator.h"

class DynamicalModel;

class Interpolator
{
public:
    typedef Integrator::Scalar Scalar;
    typedef Integrator::Point Point;
    typedef Integrator::Vector Vector;

    Interpolator(DynamicalModel* m);
    virtual ~Interpolator() {}
    Vector interpolate(const Point& p) const { return _interpolate(p); }

protected:
    DynamicalModel* model;

    /* only Interpolator is declared a friend class of DynamicalModel */
    Point &gridSpacing;
    Point &gridOffset;

    /* Implementation of the interpolation method */
    virtual Vector _interpolate(const Point& p) const = 0;
};

#endif


