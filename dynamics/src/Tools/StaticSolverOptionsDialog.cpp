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
#include "StaticSolverOptionsDialog.h"

#include "GLMotif/WidgetFactory.h"

#include "StaticSolverTool.h"

GLMotif::PopupWindow* StaticSolverOptionsDialog::createDialog()
{
   WidgetFactory factory;

   // create the popup shell
   GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", " Static Solver Options");

   // create the main layout
   GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 1);
   factory.setLayout(parameterDialog);

   // create a layout for slilder bar and associate GLMotif objects
   GLMotif::RowColumn* sliderLayout=factory.createRowColumn("SliderLayout", 3);
   factory.setLayout(sliderLayout);

   factory.createLabel("", "Number Of Points");

   // create text field
   numberOfPointsValue=factory.createTextField("NumberOfPointsTextField", 10);
   numberOfPointsValue->setString("5000");

   // create and initialize slider
   numberOfPointsSlider=factory.createSlider("NumberOfPointsSlider", 15.0);
   numberOfPointsSlider->setValueRange(50.0, 20000.0, 50.0);
   numberOfPointsSlider->setValue(5000.0);

   // set slider callback
   numberOfPointsSlider->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::sliderCallback);

   // create line style toggle buttons (check boxes)
   factory.createLabel("", "Line Type");
   GLMotif::ToggleButton* basicLineToggle=factory.createCheckBox("BasicLineToggle", "2D", true);
   GLMotif::ToggleButton* polyLineToggle=factory.createCheckBox("PolyLineToggle", "3D");

   // assign line style toggle callbacks
   basicLineToggle->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::lineStyleTogglesCallback);
   polyLineToggle->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::lineStyleTogglesCallback);

   // add line style toggles to array for radio-button behavior
   lineToggles.push_back(basicLineToggle);
   lineToggles.push_back(polyLineToggle);

   // create line color toggle buttons (check boxes)
   factory.createLabel("", "Line Color");
   GLMotif::ToggleButton* solidColorToggle=factory.createCheckBox("SolidColorToggle", "Solid");
   GLMotif::ToggleButton* gradientColorToggle=factory.createCheckBox("GradientColorToggle", "Gradient", true);

   // assign line color toggle button callbacks
   solidColorToggle->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::colorStyleTogglesCallback);
   gradientColorToggle->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::colorStyleTogglesCallback);

   // add line color toggles to array for radio-button behavior
   colorToggles.push_back(solidColorToggle);
   colorToggles.push_back(gradientColorToggle);

   // Multiple static solutions
   factory.createLabel("", "Behavior");
   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);
   GLMotif::ToggleButton* multipleStaticSolutionsToggle=factory.createCheckBox("MultipleStaticSolutionsToggle", "Allow Multiple Static Solutions", pTool->multipleStaticSolutions);
   multipleStaticSolutionsToggle->getValueChangedCallbacks().add(this, &StaticSolverOptionsDialog::multipleStaticSolutionsToggleCallback);
   factory.createLabel("Spacer0", "");
   sliderLayout->manageChild();

   factory.setLayout(parameterDialog);

   // create spacer (newline)
   factory.createLabel("Spacer1", "");

   /* Clear Static Solutions Button */
   GLMotif::RowColumn* clearButtonLayout=factory.createRowColumn("ClearButtonLayout", 2);
   factory.setLayout(clearButtonLayout);
   GLMotif::Button* clearButton=factory.createButton("ClearButton", "Clear Static Solutions");
   clearButton->getSelectCallbacks().add(this, &StaticSolverOptionsDialog::clearButtonCallback);
   factory.createLabel("Spacer2", "");
   clearButtonLayout->manageChild();

   parameterDialog->manageChild();

   return parameterDialogPopup;
}

void StaticSolverOptionsDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
   // get slider value
   unsigned int value=(unsigned int) cbData->value;

   // update text field
   char buff[10];
   snprintf(buff, sizeof(buff), "%i", value);

   numberOfPointsValue->setString(buff);

   // update static solver
   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);
   pTool->setNumberOfPoints(value);
}

void StaticSolverOptionsDialog::lineStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set the line style value

   std::string name=cbData->toggle->getName();

   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);

   if (name == "BasicLineToggle")
   {
      pTool->setLineStyle(StaticSolverData::BASIC);
   }
   else if (name == "PolyLineToggle")
   {
      pTool->setLineStyle(StaticSolverData::POLY_LINE);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=lineToggles.begin(); button
         != lineToggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
}

void StaticSolverOptionsDialog::colorStyleTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   // set the line color value

   std::string name=cbData->toggle->getName();

   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);

   if (name == "SolidColorToggle")
   {
      std::cout << "setting color SOLID" << std::endl;
      pTool->setColorStyle(StaticSolverData::SOLID);
   }
   else if (name == "GradientColorToggle")
   {
      std::cout << "setting color GRADIENT" << std::endl;
      pTool->setColorStyle(StaticSolverData::GRADIENT);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=colorToggles.begin(); button
         != colorToggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
}

void StaticSolverOptionsDialog::multipleStaticSolutionsToggleCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);
   pTool->multipleStaticSolutions = not pTool->multipleStaticSolutions;
}

void StaticSolverOptionsDialog::clearButtonCallback(GLMotif::Button::SelectCallbackData* cbData)
{
   StaticSolverTool* pTool=static_cast<StaticSolverTool*> (tool);
   pTool->clearDatasets();
   pTool->requestDataDisplayListUpdate();
}
