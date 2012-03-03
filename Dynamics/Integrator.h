/*******************************************************************************
 Integrator: Base class for dynamical integrators.
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
#ifndef INTEGRATOR_H
#define INTEGRATOR_H

// Vrui includes
//
#include <GLMotif/GLMotif>
#include <Geometry/Point.h>
#include <Geometry/Vector.h>

// Forward declarations
class CaveDialog;

/** Base class for dynamical integrators.
 *
 * A dynamical system is composed of two classes. A class which describes
 * the differential equations (sub-classed from DynamicalModel) and a
 * custom integrator (sub-classed from Integrator). The integrator class
 * functions as a wrapper, containing the dynamics object as well as a
 * templated Runge-Kutta solver. For example, the integrator for the
 * Lorenz Attractor contains the following two members (taken from
 * auto-generated (LorenzAttractor.h):
 * \code
 *   LorenzAttractor* dynamics;
 *   RungeKutta4<LorenzAttractor>* integrator;
 * \endcode
 * This makes Integrator the central class and as a result the main
 * application will generally utilize an Itegrator object (or an array of
 * Integrator objects).
 */
class Integrator
{
   public:
      typedef double Scalar; // Scalar type of underlying point space
      static const int dimension=3; // Always three-dimensional
      typedef Geometry::Point<Scalar, dimension> Point; // Point type
      typedef Geometry::Vector<Scalar,dimension> Vector; // Vector type

      Integrator()
      {
      }
      virtual ~Integrator()
      {
      }

      /** Solve the dynamical equations at a specified point.
       *
       * \param p The point at which the equations are evaluated.
       * \return  The step vector.
       */
      virtual Vector step(const Point& p) = 0;

      /** Returns the model version.
       *
       * If 'model' is the instance subclassed from DynamicalModel, then we
       * typically return model.getModelVersion().
       */
      virtual unsigned int getModelVersion() = 0;

      /** Create and return a dialog for interacting with system.
       *
       * \param parent The parent menu (typically the application main menu).
       * \return User-interface dialog for the dynamical system.
       */
      virtual CaveDialog* createParameterDialog(GLMotif::PopupMenu *parent) = 0;

      /** Return the viewing center for the model.
       */
      virtual Point center() const = 0;

      virtual bool autoShowParameterDialog()
      {
        return false;
      }
};

#endif
