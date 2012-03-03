/*******************************************************************************
 DotSpreaderOptionsDialog: User interface dialog for the dot spreader tool.
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
#include "DotSpreaderOptionsDialog.h"

#include "GLMotif/WidgetFactory.h"
#include "DotSpreaderTool.h"

GLMotif::PopupWindow* DotSpreaderOptionsDialog::createDialog()
{
   WidgetFactory factory;

   // create popup-shell
   GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", "Dot Spreader Options");

   // create the main layout
   GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 3);
   factory.setLayout(parameterDialog);

   factory.createLabel("", "NumberOfParticles");

   // create text field
   numberOfParticlesValue=factory.createTextField("NumberOfParticlesTextField", 10);
   numberOfParticlesValue->setLabel("50000");

   // create and initialize slider object
   numberOfParticlesSlider=factory.createSlider("NumberOfParticlesSlider", 15.0);
   numberOfParticlesSlider->setValueRange(1000.0, 250000.0, 1000.0);
   numberOfParticlesSlider->setValue(50000.0);

   // set the slider callback
   numberOfParticlesSlider->getValueChangedCallbacks().add(this, &DotSpreaderOptionsDialog::sliderCallback);

   factory.createLabel("ParticleSizeLabel", "Particle Size");

   pointSizeValue=factory.createTextField("PointSizeTextField", 10);
   pointSizeValue->setLabel("0.1");

   pointSizeSlider=factory.createSlider("PointSizeSlider", 15.0);
   pointSizeSlider->setValueRange(0.01, 1.0, 0.05);
   pointSizeSlider->setValue(0.1);

   pointSizeSlider->getValueChangedCallbacks().add(this, &DotSpreaderOptionsDialog::sliderCallback);

   // create distribution check boxes
   GLMotif::ToggleButton* surfaceDistributionToggle=factory.createCheckBox("SurfaceDistributionToggle", "Surface", true);
   GLMotif::ToggleButton* volumeDistributionToggle=factory.createCheckBox("VolumeDistributionToggle", "Volume");

   // set callbacks for toggle buttons (check boxes)
   surfaceDistributionToggle->getValueChangedCallbacks().add(this, &DotSpreaderOptionsDialog::distributionTogglesCallback);
   volumeDistributionToggle->getValueChangedCallbacks().add(this, &DotSpreaderOptionsDialog::distributionTogglesCallback);

   // add toggle buttons to array for radio-button behavior
   distributionToggles.push_back(surfaceDistributionToggle);
   distributionToggles.push_back(volumeDistributionToggle);

   // create push buttons
   clearParticles = factory.createButton("ClearParticles", "Clear Particles");

   // assign callbacks for buttons
   clearParticles->getSelectCallbacks().add(this, &DotSpreaderOptionsDialog::buttonCallback);

   parameterDialog->manageChild();

   return parameterDialogPopup;
}

void DotSpreaderOptionsDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
   // get slider value
   float value=cbData->value;

   // update text field
   char buff[10];

   // update dot spreader
   DotSpreaderTool* pTool=static_cast<DotSpreaderTool*> (tool);

   std::string name=cbData->slider->getName();

   if (name == "NumberOfParticlesSlider")
   {
      pTool->setNumberOfParticles((unsigned int) value);

      snprintf(buff, sizeof(buff), "%i", (unsigned int) value);
      numberOfParticlesValue->setLabel(buff);
   }
   else if (name == "PointSizeSlider")
   {
      pTool->setPointSize(value);

      snprintf(buff, sizeof(buff), "%.2f", value);
      pointSizeValue->setLabel(buff);
   }
   else
   {
   }
}

void DotSpreaderOptionsDialog::distributionTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set the dot spreader distribution method

   std::string name=cbData->toggle->getName();

   DotSpreaderTool* pTool=static_cast<DotSpreaderTool*> (tool);

   if (name == "SurfaceDistributionToggle")
   {
      pTool->setDistributionMethod(DotSpreaderData::SURFACE);
   }
   else if (name == "VolumeDistributionToggle")
   {
      pTool->setDistributionMethod(DotSpreaderData::VOLUME);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=distributionToggles.begin(); button
         != distributionToggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
}

void DotSpreaderOptionsDialog::buttonCallback(GLMotif::Button::SelectCallbackData* cbData)
{
   std::string name = cbData->button->getName();
   DotSpreaderTool* pTool=static_cast<DotSpreaderTool*> (tool);
   if (name == "ClearParticles")
   {
       pTool->clearParticles();
   }
   else 
   {
    std::cout << name << std::endl;
   }
}

