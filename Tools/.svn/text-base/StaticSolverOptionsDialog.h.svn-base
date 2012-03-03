/*******************************************************************************
 StaticSolverOptionsDialog: User interface dialog for the static solver tool.
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
#ifndef STATIC_SOLVER_OPTIONS_DIALOG_H
#define STATIC_SOLVER_OPTIONS_DIALOG_H

#include <GLMotif/GLMotif>
#include "CaveDialog.h"

#include "AbstractDynamicsTool.h"

/** User-interface dialog for setting StaticSolverTool options.
 */
class StaticSolverOptionsDialog: public CaveDialog
{
      typedef std::vector<GLMotif::ToggleButton*> ToggleArray;

      AbstractDynamicsTool* tool;

      GLMotif::Slider* numberOfPointsSlider;
      GLMotif::TextField* numberOfPointsValue;

      void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
      void lineStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void colorStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void multipleStaticSolutionsToggleCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void clearButtonCallback(GLMotif::Button::SelectCallbackData* cbData);

      ToggleArray lineToggles;
      ToggleArray colorToggles;

   protected:
      GLMotif::PopupWindow* createDialog();

   public:
      StaticSolverOptionsDialog(GLMotif::PopupMenu *parentMenu, AbstractDynamicsTool *t) :
         CaveDialog(parentMenu), tool(t)
      {
         dialogWindow=createDialog();
      }

      virtual ~StaticSolverOptionsDialog()
      {
      }
};

#endif
