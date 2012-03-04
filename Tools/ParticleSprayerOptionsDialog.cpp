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
#include "ParticleSprayerOptionsDialog.h"

#include "GLMotif/WidgetFactory.h"
#include "ParticleSprayerTool.h"

GLMotif::PopupWindow* ParticleSprayerOptionsDialog::createDialog()
{
   WidgetFactory factory;

   // create popup-shell
   GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", "Particle Sprayer Options");

   // create the main layout
   GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 1);
   factory.setLayout(parameterDialog);

   // create a layout for slider bar and associated GLMotif objects
   GLMotif::RowColumn* sliderLayout=factory.createRowColumn("SliderLayout", 3);
   factory.setLayout(sliderLayout);

   factory.createLabel("", "Lifetime");

   // create text field
   lifetimeValue=factory.createTextField("LifetimeTextField", 10);
   lifetimeValue->setString("750");

   // create and initialize slider object
   lifetimeSlider=factory.createSlider("LifetimeSlider", 15.0);
   lifetimeSlider->setValueRange(100.0, 3000.0, 50.0);
   lifetimeSlider->setValue(750.0);

   // set slider callback
   lifetimeSlider->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::sliderCallback);

   factory.createLabel("", "Spread");

   // create text field
   emitterSpreadValue=factory.createTextField("EmitterSpreadTextField", 10);
   emitterSpreadValue->setString("0.5");

   // create and initialize slider object
   emitterSpreadSlider=factory.createSlider("EmitterSpreadSlider", 15.0);
   emitterSpreadSlider->setValueRange(0.0, 1.0, 0.05);
   emitterSpreadSlider->setValue(0.5);

   // set slider callback
   emitterSpreadSlider->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::sliderCallback);

   factory.createLabel("ParticleSizeLabel", "Particle Size");

   pointSizeValue=factory.createTextField("PointSizeTextField", 10);
   pointSizeValue->setString("0.05");

   pointSizeSlider=factory.createSlider("PointSizeSlider", 15.0);
   pointSizeSlider->setValueRange(0.01, 0.25, 0.01);
   pointSizeSlider->setValue(0.05);

   pointSizeSlider->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::sliderCallback);

   sliderLayout->manageChild();

   factory.setLayout(parameterDialog);

   // create layout for check boxes
   GLMotif::RowColumn* actionTogglesLayout=factory.createRowColumn("ActionTogglesLayout", 4);
   factory.setLayout(actionTogglesLayout);

   // create action check boxes
   GLMotif::ToggleButton* sprayActionToggle=factory.createCheckBox("SprayActionToggle", "Spray Particles", true);
   GLMotif::ToggleButton* createEmitterActionToggle=factory.createCheckBox("CreateEmitterActionToggle", "Create Emitter");
   GLMotif::ToggleButton* moveEmitterActionToggle=factory.createCheckBox("MoveEmitterActionToggle", "Move Emitter");
   GLMotif::ToggleButton* deleteEmitterActionToggle=factory.createCheckBox("DeleteEmitterActionToggle", "Delete Emitter");

   // set callbacks for toggle buttons (check boxes)
   sprayActionToggle->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::actionTogglesCallback);
   createEmitterActionToggle->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::actionTogglesCallback);
   moveEmitterActionToggle->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::actionTogglesCallback);
   deleteEmitterActionToggle->getValueChangedCallbacks().add(this, &ParticleSprayerOptionsDialog::actionTogglesCallback);

   // add toggle buttons to array for radio-button behavior
   actionToggleButtons.push_back(sprayActionToggle);
   actionToggleButtons.push_back(createEmitterActionToggle);
   actionToggleButtons.push_back(moveEmitterActionToggle);
   actionToggleButtons.push_back(deleteEmitterActionToggle);

   // create a push button for clearing objects
   clearParticles = factory.createButton("ClearParticles", "Clear Particles");
   clearEmitters = factory.createButton("ClearEmitters", "Clear Emitters");

   // assign callback
   clearEmitters->getSelectCallbacks().add(this, &ParticleSprayerOptionsDialog::buttonCallback);
   clearParticles->getSelectCallbacks().add(this, &ParticleSprayerOptionsDialog::buttonCallback);

   actionTogglesLayout->manageChild();

   parameterDialog->manageChild();

   return parameterDialogPopup;
}

void ParticleSprayerOptionsDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
   // get slider value
   float value=cbData->value;

   char buff[10];

   ParticleSprayerTool* pTool=static_cast<ParticleSprayerTool*> (tool);

   std::string name=cbData->slider->getName();

   if (name == "LifetimeSlider")
   {
      // update text field
      snprintf(buff, sizeof(buff), "%i", (unsigned int) value);
      lifetimeValue->setString(buff);

      // update particle sprayer
      pTool->setLifetime((unsigned int) value);
   }
   else if (name == "EmitterSpreadSlider")
   {
      // update text field
      snprintf(buff, sizeof(buff), "%.2f", value);
      emitterSpreadValue->setString(buff);

      // update particle sprayer
      pTool->setEmitterSpread(value);
   }
   else if (name == "PointSizeSlider")
   {
      pTool->setPointSize(value);

      snprintf(buff, sizeof(buff), "%.2f", value);
      pointSizeValue->setString(buff);
   }
   else
   {
   }
}

void ParticleSprayerOptionsDialog::actionTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set the particle sprayer action

   std::string name=cbData->toggle->getName();

   ParticleSprayerTool* pTool=static_cast<ParticleSprayerTool*> (tool);

   if (name == "SprayActionToggle")
   {
      pTool->setAction(ParticleSprayerData::SPRAY_PARTICLES);
   }
   else if (name == "CreateEmitterActionToggle")
   {
      pTool->setAction(ParticleSprayerData::CREATE_EMITTER);
   }
   else if (name == "MoveEmitterActionToggle")
   {
      pTool->setAction(ParticleSprayerData::MOVE_EMITTER);
   }
   else if (name == "DeleteEmitterActionToggle")
   {
      pTool->setAction(ParticleSprayerData::DELETE_EMITTER);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=actionToggleButtons.begin(); button
         != actionToggleButtons.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
}

void ParticleSprayerOptionsDialog::buttonCallback(GLMotif::Button::SelectCallbackData* cbData)
{
   std::string name = cbData->button->getName();
   ParticleSprayerTool* pTool=static_cast<ParticleSprayerTool*> (tool);
   if (name == "ClearParticles")
   {
       pTool->clearParticles();
   }
   else if (name == "ClearEmitters")
   {
       pTool->clearEmitters();
   }
   else 
   {
   }
}
