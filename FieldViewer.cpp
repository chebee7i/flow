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
#include "FieldViewer.h"

// Vrui includes
//
#include <Images/RGBImage.h>
#include <Images/ReadImageFile.h>

// ToolBox includes
//
#include <ToolBox/ToolBox>
#include <ToolBox/Extensions/ToolRotator.h>

// Project includes
//
#include "Dynamics/IntegrationStepSize.h"
#include "Directory.h"

#include <algorithm>

DynamicsFactory Factory;

Viewer::Viewer(int &argc, char** argv, char** appDefaults) :
   Vrui::Application(argc, argv, appDefaults), tools(ToolList()), model(NULL),
   frameRateDialog(NULL),
   positionDialog(NULL),
   parameterDialog(NULL),
   currentOptionsDialog(NULL),
   optionsDialogs(DialogArray()),
   elapsedTime(0.0), stepSizeVersion(0), modelVersion(0),
   masterout(std::cout), nodeout(std::cout), debugout(std::cerr)
{
   // load ToolBox
   ToolBox::ToolBoxFactory::instance();

   // load dynamics plugins
   try {
   	model_names=loadPlugins();
   	}
   catch (std::runtime_error& e) {
   	 std::cerr << "ERROR: " << e.what() << std::endl;
   }
   catch (...) {
   	 std::cerr << "UNDEFINED ERROR." << std::endl;
   }

   // alphabetize the names list
   std::sort(model_names.begin(), model_names.end());

   // initialize the model dynamics (first one alphabetically)
   //model=Factory[model_names[0]]();
   // load lorenz
   model = Factory["Lorenz Attractor"]();

   // create and set the main menu
   mainMenu=createMainMenu();
   Vrui::setMainMenu(mainMenu);

   // create other dialogs
   frameRateDialog = new FrameRateDialog(mainMenu);
   positionDialog = new PositionDialog(mainMenu);

   // create and assign associated parameter dialog
   parameterDialog=model->createParameterDialog(mainMenu);

   // center the display
   resetNavigationCallback(0);
}

Viewer::~Viewer()
{
   delete mainMenu;
   delete parameterDialog;
   delete model;
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
      delete *tool;

   // close all dynamic libs (plugins)
   for (DLList::iterator lib=dl_list.begin(); lib != dl_list.end(); ++lib)
      dlclose(*lib);
}

//
// Internal methods
//

void Viewer::setRadioToggles(ToggleArray& toggles, const std::string& name)
{
   for (ToggleArray::iterator button=toggles.begin(); button != toggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
}

std::vector<std::string> Viewer::loadPlugins() throw(std::runtime_error)
{
   nodeout() << "Loading plugins..." << std::endl;

   // create Directory object and parse plugin directory
   std::string directory(RESOURCEDIR);
   directory += "/plugins";
   
   Directory dir;
   dir.addExtensionFilter("so");
   dir.read(directory);

   void *dlib;
   for (std::vector<std::string>::const_iterator lib=dir.contents().begin(); lib
         != dir.contents().end(); ++lib)
   {
      nodeout() << "  opening " << *lib << "..." << std::endl;

      // prepend directory name to library file name
      std::string file=directory + "/" + *lib;

      // open the library
      dlib=dlopen(file.c_str(), RTLD_NOW);
      //dlib=dlopen(file.c_str(), RTLD_LAZY|RTLD_GLOBAL);

      if (dlib == NULL)
         throw std::runtime_error(dlerror());

      // add the handle to our list
      dl_list.insert(dl_list.end(), dlib);
   }

   // create an array of model names
   std::vector<std::string> model_names;

   for (DynamicsFactory::iterator itr=Factory.begin(); itr != Factory.end(); ++itr)
   {
      model_names.insert(model_names.end(), itr->first);
   }

   // return the name array
   return model_names;
}

//
// Interface
//

void Viewer::initContext(GLContextData& contextData) const
{
   // create data item and add it to the context data
   DTS::DataItem* dataItem=new DTS::DataItem;
   contextData.addDataItem(this, dataItem);

   // get point sprite image
   std::string directory(RESOURCEDIR);
   Images::RGBImage spriteTexture=Images::readImageFile((directory+"/particle.png").c_str());

   // initialize texture parameters
   glBindTexture(GL_TEXTURE_2D, dataItem->spriteTextureObjectId);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   spriteTexture.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB);

   glBindTexture(GL_TEXTURE_2D, 0);
}

void Viewer::display(GLContextData& contextData) const
{
   // get data item from context data
   DTS::DataItem* dataItem=contextData.retrieveDataItem<DTS::DataItem> (this);

   // iterator over all tools and render data
   for (ToolList::const_iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
      if ((*tool)->isDisabled())
         continue;
      else if ( (*tool)->needsGLSL() && not dataItem->hasShaders )
      {
         continue;
      }
      else
      {
         (*tool)->render(dataItem);
      }
   }
}

void Viewer::frame()
{
   // check if the integrator has updated
   unsigned int _stepSizeVersion = IntegrationStepSize::instance()->getValueVersion();
   unsigned int _modelVersion = model->getModelVersion();
   bool updatedIntegrator = false;
   if (stepSizeVersion != _stepSizeVersion || modelVersion != _modelVersion)
   {
      updatedIntegrator = true;
   }

   // frame rate
   double frameTime = Vrui::getCurrentFrameTime();
   double throttledFrameRate = frameRateDialog->getThrottledFrameRate();
   frameRateDialog->setFrameRate(1.0/frameTime);
   elapsedTime += frameTime;

   bool stepTools = false;
   if (elapsedTime >= 1.0/throttledFrameRate)
   {
      stepTools = true;
      //std::cout << "throttled value reached" << std::endl;
      elapsedTime = 0.0;
   }

   // iterate over all tools and do required processing
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
     if ((*tool)->isDisabled())
     {
       continue;
     }
     else
     {
       if (updatedIntegrator)
       {
          (*tool)->updatedIntegrator();
       }
       if (stepTools)
       {
         (*tool)->step();
       }

       // If we want multiple users to be able to track various tools,
       // we'll need to go back to the TrackerTool creation. For single
       // position tracking, we just track the position of the first tool.
       if (tool == tools.begin())
       {
          Vrui::Point position = tools[0]->toolBox()->deviceTransformationInModel().getOrigin();//Vrui::getDeviceTransformation(input.getDevice(0)).getOrigin();
          positionDialog->setPosition(position);
       }
     }
   }

   // mark versions the same
   stepSizeVersion = _stepSizeVersion;
   modelVersion = _modelVersion;

   // update the display
   Vrui::requestUpdate();
}

void Viewer::updateToolToggles()
{
   // loop over toggle buttons
   for (ToggleArray::iterator button=toolsToggleButtons.begin(); button
         != toolsToggleButtons.end(); ++button)
   {
      // get toggle button name
      std::string toggle_name=(*button)->getName();

      // erase "toggle" from name
      toggle_name.erase(toggle_name.size() - 6, toggle_name.size());

      toggle_name+="Tool";

      // use the toolmap (names:tools) to get the tool pointer
      AbstractDynamicsTool* tool=toolmap[toggle_name];

      // set the toggle button state based on tool state
      bool state=tool->isDisabled();

      (*button)->setToggle(!state);
   }
}

void Viewer::updateCurrentOptionsDialog()
{
   // get the current tool from toolbox
   ToolBox::Tool* currentTool=tools[0]->toolBox()->currentTool();

   if (currentTool == NULL)
   {
      nodeout() << "ERROR::currentTool not defined." << std::endl;
      return; // I feel better already...
   }

   // find the tool in the application tool list
   int index=0;
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
      if (*tool == currentTool)
         break;
      index++;
   }

   bool popup=false;
   bool reset=false;
   GLMotif::WidgetManager::Transformation oldTrans;

   // if tool options dialog has not been defined set the dialog and return
   if (currentOptionsDialog == NULL)
   {
      currentOptionsDialog=optionsDialogs[index];
      return;
   }

   // save the transformation of existing dialog
   oldTrans=currentOptionsDialog->getTransformation();

   // hide dialog before reassigning
   if (currentOptionsDialog->state() == CaveDialog::ACTIVE)
   {
      currentOptionsDialog->hide();
      popup=true;
   }

   if (!currentOptionsDialog->initialized())
   {
      reset=true;
   }

   // set dialog to one associated with current tool
   currentOptionsDialog=optionsDialogs[index];

   if (!reset)
      currentOptionsDialog->setTransformation(oldTrans);
   else
      currentOptionsDialog->reset();

   // popup if necessary
   if (popup)
      currentOptionsDialog->show();
}

//
// Callbacks
//

void Viewer::toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData)
{
   ToolBox::ToolBox* toolBox=dynamic_cast<ToolBox::ToolBox*> (cbData->tool);

   if (toolBox != 0)
   {
      (new ToolBox::Extensions::ToolRotator(toolBox))->alwaysShowCurrent(false).positionOfCurrent(180).closeDelay(2.0);

      masterout() << "Creating tools and adding to toolbox..." << std::endl;

      AbstractDynamicsTool *tool;

      // add tools

      masterout() << "\tAdding Static Solver..." << std::endl;

      tool=new StaticSolverTool(toolBox, this);
      // set dynamical integrator and add tool to array
      tool->setIntegrator(model);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["StaticSolverTool"]=tool;

      masterout() << "\tAdding Dot Spreader..." << std::endl;

      tool=new DotSpreaderTool(toolBox, this);
      // set dynamical integrator and add tool to array
      tool->setIntegrator(model);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["DotSpreaderTool"]=tool;

      masterout() << "\tAdding Particle Sprayer..." << std::endl;

      tool=new ParticleSprayerTool(toolBox, this);
      // set dynamical integrator and add tool to array
      tool->setIntegrator(model);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["ParticleSprayerTool"]=tool;

      masterout() << "\tAdding Dynamic Solver..." << std::endl;

      tool=new DynamicSolverTool(toolBox, this);
      // set dynamical integrator and add tool to array
      tool->setIntegrator(model);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["DynamicSolverTool"]=tool;

      // automatically load the first tool and set options dialog
      AbstractDynamicsTool* currentTool = static_cast<AbstractDynamicsTool*>(tools.front());
      currentTool->grab();
      updateCurrentOptionsDialog();
   }
}

void Viewer::toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData)
{
      // need to fix this to handle multiple users each with their own toolbox
      tools.clear();
      toolmap.clear();
}

void Viewer::resetNavigationCallback(Misc::CallbackData* cbData)
{
   Vrui::setNavigationTransformation(model->center(), 40.0);
}

void Viewer::mainMenuTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData *cbData)
{
   std::string name=cbData->toggle->getName();

   if (name == "ShowParameterDialogToggle")
   {
      // if toggle is set show the parameter dialog
      if (cbData->toggle->getToggle())
      {
         parameterDialog->show();
      }
      // otherwise hide the parameter dialog
      else
      {
         parameterDialog->hide();
      }
   }
   else if (name == "ShowOptionsDialogsToggle")
   {
      // if toggle is set show tool options dialogs
      if (cbData->toggle->getToggle())
      {
         // if no tool is set reset toggle and return
         if (!currentOptionsDialog)
         {
            cbData->toggle->setToggle(false);
            return;
         }

         currentOptionsDialog->show();

      }
      // otherwise hide the current dialog
      else
      {
         currentOptionsDialog->hide();
      }
   }
   else if (name == "ShowFrameRateDialogToggle")
   {
      // if toggle is set show the parameter dialog
      if (cbData->toggle->getToggle())
      {
         frameRateDialog->show();
      }
      // otherwise hide the parameter dialog
      else
      {
         frameRateDialog->hide();
      }
   }
   else if (name == "ShowPositionDialog")
   {
      // if toggle is set to show the position dialog
      if (cbData->toggle->getToggle())
      {
         positionDialog->show();
      }
      else
      {
         positionDialog->hide();
      }
   }
   else
   {
   }
}

void Viewer::dynamicsMenuCallback(GLMotif::ToggleButton::ValueChangedCallbackData *cbData)
{
   bool popup=false;

   // delete current dynamical model
   if (model != NULL)
      delete model;

   GLMotif::WidgetManager::Transformation oldTrans;

   // delete current parameter dialog
   if (parameterDialog != NULL)
   {
      // save the old transformation
      oldTrans=parameterDialog->getTransformation();

      // if dialog is currently shown hide before deleting
      if (parameterDialog->state() == CaveDialog::ACTIVE)
      {
         parameterDialog->hide();

         // set flag to popup after creating new dialog
         popup=true;
      }

      delete parameterDialog;
   }

   std::string name=cbData->toggle->getName();

   // create the key from the toggle name
   std::string key(name);
   key.erase(key.find("toggle"));

   debugout() << "  toggle: " << name << "\tkey: " << key << std::endl;

   // create the dynamical model
   model=Factory[key]();

   // create/assign parameter dialog
   parameterDialog=model->createParameterDialog(mainMenu);

   parameterDialog->setTransformation(oldTrans);

   // popup parameter dialog if previously shown or system requests it
   if (popup)
      parameterDialog->show();
   else if (model->autoShowParameterDialog())
   {
      showParameterDialogToggle->setToggle(true);
      parameterDialog->show();
   }

   // update the integration step size
   double step_size=IntegrationStepSize::instance()->getSavedValue(key);

   if (step_size > 0.0)
   {
      masterout() << key << ":restoring step size [" << step_size << "]"
                  << std::endl;
   }
   else
   {
      // set the integration step size to the default value
      step_size=IntegrationStepSize::DefaultStepSize;
      masterout() << key << ":integration step size not set."
                  << " Setting to default (" << step_size << ")" << std::endl;
   }

   IntegrationStepSize::instance()->setValue(step_size);

   // iterate through tools and sets dynamical integrator
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
      (*tool)->setIntegrator(model);
   }

   // fake radio-button behavior
   setRadioToggles(dynamicsToggleButtons, name);
}

void Viewer::toolsMenuCallback(GLMotif::ToggleButton::ValueChangedCallbackData *cbData)
{
   AbstractDynamicsTool* tool;

   std::string name=cbData->toggle->getName();
   {
      if (name == "ParticleSprayerToggle")
      {
         masterout() << "Current Tool: Particle Sprayer" << std::endl;

         tool=toolmap["ParticleSprayerTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);

      }
      else if (name == "DotSpreaderToggle")
      {
         masterout() << "Current Tool: Dot Spreader" << std::endl;

         tool=toolmap["DotSpreaderTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
      }
      else if (name == "StaticSolverToggle")
      {
         masterout() << "CurrentTool: Static Solver" << std::endl;

         tool=toolmap["StaticSolverTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
      }
      else if (name == "DynamicSolverToggle")
      {
         masterout() << "CurrentTool: Dynamic Solver" << std::endl;

         tool=toolmap["DynamicSolverTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
      }
      else
      {
      }
   }

}
