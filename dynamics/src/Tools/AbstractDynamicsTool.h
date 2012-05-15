/*******************************************************************************
 AbstractDynamicsTool: Base class for dynamics tools.
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
#ifndef ABSTRACT_DYNAMICS_TOOL_H
#define ABSTRACT_DYNAMICS_TOOL_H

// Vrui includes
//
#include <Vrui/Vrui>

// External includes
//
#include <ToolBox/ToolBox>

// Project includes
//
#include "Dynamics/Experiment.h"
#include "CaveDialog.h"

// Haven't yet decided how/where to make this globally available
typedef double Scalar;
typedef Experiment<Scalar> DTSExperiment;


// Forward declarations
class Viewer;

namespace DTS
{
class DataItem;
}

/** Abstract base class for dynamics tools.
 */
class AbstractDynamicsTool: public ToolBox::Tool
{
   protected:
      ToolBox::ToolBox* toolbox;
      Viewer* application;

      DTSExperiment* experiment;
      CaveDialog* dialog;

      Vrui::Point pos;
      bool disabled;
      bool _needsGLSL;

   public:

      /* Interface */

      AbstractDynamicsTool(ToolBox::ToolBox* toolBox, Viewer* app) :
         Tool(toolBox), toolbox(toolBox), application(app), experiment(0),
               disabled(false), _needsGLSL(true)
      {
      }

      virtual ~AbstractDynamicsTool()
      {
         delete dialog;
      }

      virtual void render(DTS::DataItem* dataItem) const = 0;
      virtual void step() = 0;

      /* ToolBox::Tool methods */
      virtual void moved(const ToolBox::MotionEvent & motionEvent) = 0;
      virtual void mainButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent) = 0;
      virtual void mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent) = 0;
      virtual void otherButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent) = 0;
      virtual void otherButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent) = 0;
      void grabbed(const ToolBox::ToolGrabEvent & toolGrabEvent);

      /* New methods */

      virtual void setExperiment(DTSExperiment* e)
      {
         experiment = e;
      }

      /* Whenever experiment is updated, this function is called. */
      virtual void updatedExperiment() 
      {
      }

      /** Create and return a dialog for interacting with tool.
       *
       * \param parentMenu The parent of the tool dialog (typically the application main menu).
       * \return User-interaction dialog for the tool.
       */
      virtual CaveDialog
            * createOptionsDialog(GLMotif::PopupMenu *parentMenu) = 0;

      /** Disable/enable tool.
       */
      void setDisabled(bool flag)
      {
         disabled=flag;
         if (flag)
            drop(); // this works because there is only one active tool
      }

      /** Return true if tool is currently disabled.
       */
      bool isDisabled() const
      {
         return disabled;
      }

      /** Return true if tool requires GLSL.
       */
      bool needsGLSL() const
      {
         return _needsGLSL;
      }


};  

#endif
