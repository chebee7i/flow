/*******************************************************************************
 ParticleSprayerOptionsDialog: User interface dialog for the particle sprayer
 tool.
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
#ifndef PARTICLE_SPRAYER_OPTIONS_DIALOG_H
#define PARTICLE_SPRAYER_OPTIONS_DIALOG_H

#include <GLMotif/GLMotif>
#include "CaveDialog.h"

#include "AbstractDynamicsTool.h"

/** User-interface dialog for setting ParticleSprayerTool options.
 */
class ParticleSprayerOptionsDialog: public CaveDialog
{
      typedef std::vector<GLMotif::ToggleButton*> ToggleArray;

      AbstractDynamicsTool* tool;

      GLMotif::Slider* lifetimeSlider;
      GLMotif::TextField* lifetimeValue;

      GLMotif::Slider* emitterSpreadSlider;
      GLMotif::TextField* emitterSpreadValue;

      GLMotif::Slider* pointSizeSlider;
      GLMotif::TextField* pointSizeValue;

      GLMotif::Button* clearParticles;
      GLMotif::Button* clearEmitters;

      void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
      void actionTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
      void buttonCallback(GLMotif::Button::SelectCallbackData* cbData);

      ToggleArray actionToggleButtons;

   protected:
      GLMotif::PopupWindow* createDialog();

   public:
      ParticleSprayerOptionsDialog(GLMotif::PopupMenu *parentMenu, AbstractDynamicsTool *t) :
         CaveDialog(parentMenu), tool(t)
      {
         dialogWindow=createDialog();
      }

      virtual ~ParticleSprayerOptionsDialog()
      {
      }

};

#endif
