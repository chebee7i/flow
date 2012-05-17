/*******************************************************************************
 FieldViewer_ui: User interface for the Dynamics Toolset application class.
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
#include "FieldViewer.h"

// Vrui includes
//
#include "GLMotif/WidgetFactory.h"

GLMotif::PopupMenu* Viewer::createMainMenu()
{
   WidgetFactory factory;

   // create the top-level shell
   GLMotif::PopupMenu* mainMenuPopup=factory.createPopupMenu("MainPopupMenu", "Viewer Options");

   // create button array inside top-level shell
   GLMotif::Menu* mainMenu=factory.createMenu("MainMenu");

   // add buttons and assign callbacks

   // create a cascade button for selecting the dynamical system
   GLMotif::CascadeButton* dynamicsTogglesCascade=factory.createCascadeButton("DynamicsTogglesCascade", "Experiments");
   dynamicsTogglesCascade->setPopup(createDynamicsTogglesMenu());

   // create a cascade button for selecting the dynamical analysis tool
   GLMotif::CascadeButton* toolsTogglesCascade=factory.createCascadeButton("ToolTogglesCascade", "Tools");
   toolsTogglesCascade->setPopup(createToolsTogglesMenu());

   // create a toggle button for showing/hiding the frame rate dialog
   GLMotif::ToggleButton* showFrameRateDialog=factory.createToggleButton("ShowFrameRateDialogToggle", "Show Frame Rate Dialog");
   showFrameRateDialog->getSelectCallbacks().add(this, &Viewer::mainMenuTogglesCallback);

   // create a toggle button for showing/hiding the position dialog
   GLMotif::ToggleButton* showPositionDialog=factory.createToggleButton("ShowPositionDialog", "Show Position Dialog");
   showPositionDialog->getSelectCallbacks().add(this, &Viewer::mainMenuTogglesCallback);

   // create a toggle button for showing/hiding the dynamical parameter interface dialog
   showParameterDialogToggle=factory.createToggleButton("ShowParameterDialogToggle", "Show Experiment Options");
   showParameterDialogToggle->getSelectCallbacks().add(this, &Viewer::mainMenuTogglesCallback);

   // create a toggle for showing/hiding the tool option dialogs
   GLMotif::ToggleButton* showOptionsDialogs=factory.createToggleButton("ShowOptionsDialogsToggle", "Show Tool Options");

   showOptionsDialogs->getSelectCallbacks().add(this, &Viewer::mainMenuTogglesCallback);

   // create a push button for reseting the view
   GLMotif::Button* resetNavigationButton=factory.createButton("ResetNavigationButton", "Reset Navigation");
   resetNavigationButton->getSelectCallbacks().add(this, &Viewer::resetNavigationCallback);

   mainMenu->manageChild();

   return mainMenuPopup;
}

GLMotif::Popup* Viewer::createDynamicsTogglesMenu()
{
   WidgetFactory factory;

   // create the top-level shell
   GLMotif::Popup* dynamicsTogglesMenuPopup=factory.createPopupMenu("DynamicsTogglesMenuPopup");

   // create button array inside top-level shell
   GLMotif::RowColumn* dynamicsTogglesMenu=factory.createMenu("DynamicsTogglesMenu");

   // for each of the experiment names
   for (std::vector<std::string>::const_iterator name=experiment_names.begin(); name!=experiment_names.end(); ++name)
   {
      size_t iIndex = (*name).rfind("Hidden");
      if( iIndex != std::string::npos )
      {
         // then while loaded, we do not make this experiment available to the user
         continue;
      }

      // create toggle button
      std::string toggle_name=(*name) + "toggle";
      GLMotif::ToggleButton* toggle=factory.createToggleButton(const_cast<char*> (toggle_name.c_str()), const_cast<char*> ((*name).c_str()));

      // turn on the first toggle
      //if (name == experiment_names.begin())
      //   toggle->setToggle(true);

      toggle->getValueChangedCallbacks().add(this, &Viewer::dynamicsMenuCallback);
      dynamicsToggleButtons.push_back(toggle);

   }

   dynamicsTogglesMenu->manageChild();

   return dynamicsTogglesMenuPopup;
}

GLMotif::Popup* Viewer::createToolsTogglesMenu()
{
   WidgetFactory factory;

   // create the top-level shell
   GLMotif::Popup* toolsTogglesMenuPopup=factory.createPopupMenu("ToolsTogglesMenuPopup");

   // create array of buttons inside top-level shell
   GLMotif::RowColumn* toolsTogglesMenu=factory.createMenu("VisualizationTogglesMenu");

   // add toggle buttons for various analysis tools
   GLMotif::ToggleButton* particleSprayerToggle=factory.createToggleButton("ParticleSprayerToggle", "Particle Sprayer", true);
   GLMotif::ToggleButton* dotSpreaderToggle=factory.createToggleButton("DotSpreaderToggle", "Dot Spreader", true);
   GLMotif::ToggleButton* staticSolverToggle=factory.createToggleButton("StaticSolverToggle", "Static Solver", true);
   GLMotif::ToggleButton* dynamicSolverToggle=factory.createToggleButton("DynamicSolverToggle", "Dynamic Solver", true);

   // assign callbacks for each toggle button
   particleSprayerToggle->getValueChangedCallbacks().add(this, &Viewer::toolsMenuCallback);
   dotSpreaderToggle->getValueChangedCallbacks().add(this, &Viewer::toolsMenuCallback);
   staticSolverToggle->getValueChangedCallbacks().add(this, &Viewer::toolsMenuCallback);
   dynamicSolverToggle->getValueChangedCallbacks().add(this, &Viewer::toolsMenuCallback);

   // add toggle button pointers to vector for radio-button behavior
   toolsToggleButtons.push_back(particleSprayerToggle);
   toolsToggleButtons.push_back(dotSpreaderToggle);
   toolsToggleButtons.push_back(staticSolverToggle);
   toolsToggleButtons.push_back(dynamicSolverToggle);

   toolsTogglesMenu->manageChild();

   return toolsTogglesMenuPopup;
}
