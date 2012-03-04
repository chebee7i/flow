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
#include "DynamicSolverOptionsDialog.h"

#include "GLMotif/WidgetFactory.h"
#include "DynamicSolverTool.h"

GLMotif::PopupWindow* DynamicSolverOptionsDialog::createDialog()
{
   WidgetFactory factory;

   // create the popup-shell
   GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", " Dynamic Solver Options");

   // create the main layout
   GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 1);
   factory.setLayout(parameterDialog);

   // create a layout for check boxes (toggle buttons)
   GLMotif::RowColumn* checkBoxLayout=factory.createRowColumn("CheckBoxLayout", 4);
   factory.setLayout(checkBoxLayout);

   // create line style toggle buttons (check boxes)
   factory.createLabel("", "Line Style");
   GLMotif::ToggleButton* noneLineToggle=
         factory.createCheckBox("NoneLineToggle", "None");
   GLMotif::ToggleButton* basicLineToggle=
         factory.createCheckBox("BasicLineToggle", "2D", true);
   GLMotif::ToggleButton* polyLineToggle=
         factory.createCheckBox("PolyLineToggle", "3D");

   // assign callbacks for line style toggle buttons
   noneLineToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::lineStyleTogglesCallback);
   basicLineToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::lineStyleTogglesCallback);
   polyLineToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::lineStyleTogglesCallback);

   // add line style toggles to array for radio-button behavior
   lineStyleToggles.push_back(noneLineToggle);
   lineStyleToggles.push_back(basicLineToggle);
   lineStyleToggles.push_back(polyLineToggle);

   // create head style toggle buttons (check boxes)
   factory.createLabel("", "Head Style");
   GLMotif::ToggleButton* noneHeadToggle=factory.createCheckBox("NoneHeadToggle", "None");
   GLMotif::ToggleButton* pointHeadToggle=factory.createCheckBox("PointHeadToggle", "Point");
   GLMotif::ToggleButton* sphereHeadToggle=factory.createCheckBox("SphereHeadToggle", "Sphere", true);

   // assign callbacks for head style toggle buttons
   noneHeadToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::headStyleTogglesCallback);
   pointHeadToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::headStyleTogglesCallback);
   sphereHeadToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::headStyleTogglesCallback);

   // add head style toggles to array for radio-button behavior
   headStyleToggles.push_back(noneHeadToggle);
   headStyleToggles.push_back(pointHeadToggle);
   headStyleToggles.push_back(sphereHeadToggle);

   // create color style toggle buttons (check boxes)
   factory.createLabel("", "Color Map");
   GLMotif::ToggleButton* solidColorToggle=factory.createCheckBox("SolidColorToggle", "Solid", true);
   GLMotif::ToggleButton* gradientColorToggle=factory.createCheckBox("GradientColorToggle", "Gradient");

   // assign callbacks for color style toggle buttons
   solidColorToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::colorStyleTogglesCallback);
   gradientColorToggle->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::colorStyleTogglesCallback);

   // add color style toggles to array for radio-button behavior
   colorMapToggles.push_back(solidColorToggle);
   colorMapToggles.push_back(gradientColorToggle);

   // create spacer (newline)
   factory.createLabel("Spacer", "");

   checkBoxLayout->manageChild();

   factory.setLayout(parameterDialog);

   // create layout for sliders
   GLMotif::RowColumn* sliderLayout=factory.createRowColumn("SliderLayout", 3);
   factory.setLayout(sliderLayout);

   factory.createLabel("ReleaseSizeLabel", "Number of Particles");

   releaseSizeValue=factory.createTextField("ReleaseSizeTextField", 10);
   releaseSizeValue->setString("1");

   releaseSizeSlider=factory.createSlider("ReleaseSizeSlider", 15.0);
   releaseSizeSlider->setValueRange(1, 250, 1);
   releaseSizeSlider->setValue(1);

   releaseSizeSlider->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::sliderCallback);

   factory.createLabel("ParticleSizeLabel", "Particle Size");

   pointSizeValue=factory.createTextField("PointSizeTextField", 10);
   pointSizeValue->setString("0.25");

   pointSizeSlider=factory.createSlider("PointSizeSlider", 15.0);
   pointSizeSlider->setValueRange(0.01, 0.25, 0.01);
   pointSizeSlider->setValue(0.25);

   pointSizeSlider->getValueChangedCallbacks().add(this, &DynamicSolverOptionsDialog::sliderCallback);

   // create a push button for clearing particles and assign callback
   GLMotif::Button* clearButton=factory.createButton("ClearButton", "Clear");
   clearButton->getSelectCallbacks().add(this, &DynamicSolverOptionsDialog::clearPointsCallback);

   sliderLayout->manageChild();

   parameterDialog->manageChild();

   return parameterDialogPopup;
}

void DynamicSolverOptionsDialog::lineStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set the line rendering style

   std::string name=cbData->toggle->getName();

   DynamicSolverTool* pTool=static_cast<DynamicSolverTool*> (tool);

   if (name == "NoneLineToggle")
   {
      pTool->setLineStyle(DynamicSolverData::NO_LINE);
   }
   else if (name == "BasicLineToggle")
   {
      pTool->setLineStyle(DynamicSolverData::BASIC);
   }
   else if (name == "PolyLineToggle")
   {
      pTool->setLineStyle(DynamicSolverData::POLYLINE);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=lineStyleToggles.begin(); button
         != lineStyleToggles.end(); ++button)
   {
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
      {
         (*button)->setToggle(false);
      }
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
      {
         (*button)->setToggle(true);
      }
   }
}

void DynamicSolverOptionsDialog::headStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set head rendering style

   std::string name=cbData->toggle->getName();

   DynamicSolverTool* pTool=static_cast<DynamicSolverTool*> (tool);

   if (name == "NoneHeadToggle")
   {
      pTool->setHeadStyle(DynamicSolverData::NO_HEAD);
   }
   else if (name == "PointHeadToggle")
   {
      pTool->setHeadStyle(DynamicSolverData::POINT);
   }
   else if (name == "SphereHeadToggle")
   {
      pTool->setHeadStyle(DynamicSolverData::SPHERE);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=headStyleToggles.begin(); button
         != headStyleToggles.end(); ++button)
   {
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
      {
         (*button)->setToggle(false);
      }
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
      {
         (*button)->setToggle(true);
      }
   }
}

void DynamicSolverOptionsDialog::colorStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set color style

   std::string name=cbData->toggle->getName();

   DynamicSolverTool* pTool=static_cast<DynamicSolverTool*> (tool);

   if (name == "SolidColorToggle")
   {
      pTool->setColorStyle(DynamicSolverData::SOLID);
   }
   else if (name == "GradientColorToggle")
   {
      pTool->setColorStyle(DynamicSolverData::GRADIENT);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=colorMapToggles.begin(); button
         != colorMapToggles.end(); ++button)
   {
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
      {
         (*button)->setToggle(false);
      }
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
      {
         (*button)->setToggle(true);
      }
   }
}

void DynamicSolverOptionsDialog::clearPointsCallback(GLMotif::Button::SelectCallbackData* cbData)
{
   DynamicSolverTool* pTool=static_cast<DynamicSolverTool*> (tool);
   pTool->clearPoints();
}

void DynamicSolverOptionsDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
   // get slider value
   float value=cbData->value;

   // update text field
   char buff[10];

   DynamicSolverTool* pTool=static_cast<DynamicSolverTool*> (tool);

   std::string name=cbData->slider->getName();

   if (name == "PointSizeSlider")
   {
      pTool->setPointSize(value);
      snprintf(buff, sizeof(buff), "%.2f", value);
      pointSizeValue->setString(buff);
   }
   else if (name == "ReleaseSizeSlider")
   {
      pTool->setReleaseClusterSize((unsigned int) value);
      snprintf(buff, sizeof(buff), "%i", (unsigned int) value);
      releaseSizeValue->setString(buff);
   }
   else
   {
   }

}
