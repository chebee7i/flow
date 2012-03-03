/*******************************************************************************
 BaseModel: Base class for dynamical models.
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
#ifndef BASE_MODEL_H
#define BASE_MODEL_H

// STL includes
//
#include <string>
#include <map>

// Project includes
//
#include "Integrator.h"
#include "Interpolator.h"
#include "TrilinearInterpolator.h"
#include "CaveDialog.h"


/** Base class for dynamics models.
 *
 * The DynamicalModel class defines the interface for the various dynamical
 * systems. Each system class will define its own parameters, however each
 * must implement the setValue method as well as the functor method. Classes
 * must also register themselves with the Factory object. This enables the
 * dynamical systems to be compiled as plugins and dynamically loaded by
 * the main Vrui application without the user even being aware of the details
 * regarding the various systems (or even the number of systems).
 *
 * The following code demonstrates how to register a class constructor with
 * the factory (taken from auto-generated LorenzAttractor.cpp code):
 * \code
 * extern "C"
 * {
 *    Integrator* maker()
 *    {
 *       return new LorenzAttractorIntegrator;
 *    }
 *
 *    class Proxy
 *    {
 *      public:
 *        Proxy()
 *        {
 *          Factory["Lorenz Attractor"] = maker;
 *        }
 *    };
 *
 *    Proxy p;
 * }
 * \endcode
 * Of course the dynamical systems code will, as a general rule, be automatically
 * generated prior to compiling. So the user should not have to worry about
 * implementing this.
 */
class DynamicalModel
{
public:
    typedef Integrator::Scalar Scalar;
    typedef Integrator::Point Point;
    typedef Integrator::Vector Vector;
    friend class Interpolator;

    DynamicalModel() : 
        simulateGrid(false),
        gridSpacing(Point(1,1,1)),
        gridOffset(Point(0,0,0)),
        modelVersion(0)

    {
        // default interpolator
        interpolator = new TrilinearInterpolator(this);
    }

    virtual ~DynamicalModel()
    {
        delete interpolator;
    }

      /** Assign simualation parameter value (meta)
       *
       *  \param name The name of the parameter.
       *  \param value Parameter value.
       */
      void setValue(const std::string& name, Scalar value)
      {
         _setValue(name, value);
         updateModelVersion();
      }

      /** Functor method.
       *
       * Return the differential equations for the model.
       * \param p Evaluation point.
       * \return Vector describing the differential equations at point p.
       */
      virtual Vector operator()(const Point& p) const
      {
			if (!simulateGrid)
			{
				return exact(p);
			}
			else
			{   

/*                Point q(interpolator->interpolate(p));
                Point r(exact(p));
                std::cout << q[0] << " " << q[1] << " " << q[2] << std::endl;
                std::cout << r[0] << " " << r[1] << " " << r[2] << std::endl;
                std::cout << std::endl;
*/
				return interpolator->interpolate(p);
			}
	   }

      /** Retrieve the exact value of vector field
       *
       * Return the exact differential equations for the model.
       * \param p Evaluation point.
       * \return Vector describing the differential equations at point p.
       */
		virtual Vector exact(const Point& p) const = 0;

        void setSimulate(bool value)
        {
            simulateGrid = value;
            updateModelVersion();
        }

        bool getSimulate()
        {
            return simulateGrid;
        }

        void setSpacing(int index, Scalar value)
        {
            gridSpacing[index] = value;
            updateModelVersion();
        }

        int getSpacing(const int index) const
        {
            return gridSpacing[index];
        }

      /** Retrieve the current value version
       */
      unsigned int getModelVersion()
      {
         return modelVersion;
      }

   protected:
      bool simulateGrid;
	  Point gridSpacing;
	  Point gridOffset;	
      Interpolator* interpolator;

   private:
      unsigned int modelVersion;

      void updateModelVersion()
      {
         ++modelVersion;
      }

      /** Assign simualation parameter value (implementation).
       *
       *  \param name The name of the parameter.
       *  \param value Parameter value.
       */
      virtual void _setValue(const std::string& name, Scalar value) = 0;


};

typedef Integrator *maker_t();

typedef std::map<std::string, maker_t*, std::less<std::string> > DynamicsFactory;

extern DynamicsFactory Factory; ///< Global object for creating dynamical models


#endif
