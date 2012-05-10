/*******************************************************************************
 FieldViewer: Dynamics Toolset application class.
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
#ifndef FIELD_VIEWER_H
#define FIELD_VIEWER_H

// Vrui includes
//
#include <Vrui/Application.h>
#include <GL/GLObject.h>

// STL includes
//
#include <exception>
#include <list>
#include <vector>
#include <string>

#include "Experiment.h"
#include "Factory.h"

//extern DynamicsFactory factory;

/** The main Vrui application class.
 */
class Viewer: public Vrui::Application, public GLObject
{
   public:
      /* Embedded classes */

      /* Interface */
      Viewer(int &argc, char** argv, char** appDefaults);
      virtual ~Viewer();

      void initContext(GLContextData& contextData) const;
      virtual void display(GLContextData& contextData) const;
      virtual void frame();
      
      /* Callbacks */
      void resetNavigationCallback(Misc::CallbackData* cbData);

   private:
      Experiment<Scalar> *experiment;
      
      
      typedef std::list<void*> DLList;
      DLList dl_list; ///< Dynamic library (plugin) list.
      std::vector<std::string> experiment_names; ///< Names of all experiments (obtained from plugins).

      /** Internal method for loading plugins (dlls).
       *
       * Searches the plugins directory for dynamic libraries. Each library
       * that is found is then loaded into memory. An exception is thrown
       * if a library fails to load.
       *
       * \return An array of the names of all plugins.
       */
      std::vector<std::string> loadPlugins() throw(std::runtime_error);
};

#endif
