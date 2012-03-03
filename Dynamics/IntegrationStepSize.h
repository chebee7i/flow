/*******************************************************************************
 IntegrationStepSize: Singleton class for storing the integration step size.
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
#ifndef INTEGRATION_STEP_SIZE_H
#define INTEGRATION_STEP_SIZE_H

// STL includes
//
#include <string>
#include <map>

/** Singleton object for storing the current integration step size.
 *
 * The IntegrationStepSize class is responsible for storing a the
 * current integration step size used by the Runge-Kutta solver.
 * Additionally, it saves the values for each of the various dynamical
 * models. The purpose of this is to allow the user to change the
 * step size on a per-system basis. For example, if a particular model
 * requires a small integration step size, the user can change the
 * value without affecting the other models. S/he can then switch to
 * a different set of equations without needing to reset the step size.
 */
class IntegrationStepSize
{
   public:
      /// Default integration step size
      static const double DefaultStepSize;

      /** Singleton access method.
       */
      static IntegrationStepSize* instance()
      {
         if (!_instance)
            _instance=new IntegrationStepSize;
         return _instance;
      }

      /** Return the current integration step size.
       */
      double getValue() const
      {
         return value;
      }

      /** Set the current integration step size.
       *
       * \param size New step size.
       */
      void setValue(double size)
      {
         value=size;
         updateValueVersion();
      }

      /** Save step size for given dynamical model.
       *
       * \param model Name of the dynamical model.
       * \param value Step size value.
       */
      void saveValue(std::string model, double value)
      {
         storedValues[model]=value;
      }

      /** Retrieve a previously saved value.
       *
       * \param model Name of the dynamical model
       * \return Saved step size value for model.
       */
      double getSavedValue(std::string model)
      {
         return storedValues[model];
      }

      /** Retrieve the current value version
       */
      unsigned int getValueVersion()
      {
         return valueVersion;
      }

   private:
      IntegrationStepSize() :
         value(0.01), 
         valueVersion(0),
         storedValues(std::map<std::string,double>())
      {
      }

      void updateValueVersion()
      {
         ++valueVersion;
      }

      /// Current integration step size and version
      double value;
      unsigned int valueVersion;

      /// Stored integration values
      std::map<std::string, double> storedValues;

      /// Singleton instance
      static IntegrationStepSize* _instance;
};

#endif
