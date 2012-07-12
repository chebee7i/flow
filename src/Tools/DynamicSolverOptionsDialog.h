/*******************************************************************************
 DynamicsSolverOptionsDialog: User interface dialog for the dynamics solver tool.
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
#ifndef DYNAMIC_SOLVER_OPTIONS_DIALOG_H
#define DYNAMIC_SOLVER_OPTIONS_DIALOG_H

#include <GLMotif/GLMotif>
#include "CaveDialog.h"

#include "AbstractDynamicsTool.h"

/** User-interface dialog for setting DynamicSolverTool options.
 */
class DynamicSolverOptionsDialog: public CaveDialog
{
      typedef std::vector<GLMotif::ToggleButton*> ToggleArray;

      AbstractDynamicsTool* tool;

      ToggleArray lineStyleToggles;
      ToggleArray headStyleToggles;
      ToggleArray colorMapToggles;

      GLMotif::Slider* pointSizeSlider;
      GLMotif::Slider* releaseSizeSlider;

      GLMotif::TextField* pointSizeValue;
      GLMotif::TextField* releaseSizeValue;

      void lineStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void headStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void colorStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void clearPointsCallback(GLMotif::Button::SelectCallbackData* cbData);
      void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);

   protected:
      GLMotif::PopupWindow* createDialog();

   public:
      DynamicSolverOptionsDialog(GLMotif::PopupMenu *parentMenu, AbstractDynamicsTool *t) :
         CaveDialog(parentMenu), tool(t)
      {
         dialogWindow=createDialog();
      }

      virtual ~DynamicSolverOptionsDialog()
      {
      }
};

#endif
