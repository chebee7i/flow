/*******************************************************************************
 ColorPoint: Simple class for describing a particle's position and color.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

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
#ifndef COLOR_POINT_H
#define COLOR_POINT_H

// Vrui includes
//
#include <Geometry/Point.h>
#include <GL/GLColor.h>

/** Data structure describing the color and position of a particle.
 */
struct ColorPoint
{
      /** Constructor.
       */
      ColorPoint() :
         color(GLColor<GLubyte, 4> ()), pos(Geometry::Point<float,3>())
      {
         color[3]=255; //default alpha value = 1.0 (opaque)
      }

      GLColor<GLubyte, 4> color; ///< The color (rgba) of the particle.
      Geometry::Point<float,3> pos; ///< The position of the particle.
};

#endif
