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

#include <algorithm>
#include <iostream>
#include <cmath>

#include <Vrui/Vrui.h>
#include <Vrui/Geometry.h>
#include <IO/OpenFile.h>
#include <IO/StandardDirectory.h>
#include <Vrui/DisplayState.h>
#include <GL/gl.h>
#include <GL/GLGeometryWrappers.h>
#include <GL/GLTransformationWrappers.h>

// Vrui includes
//
#include <Images/RGBImage.h>
#include <Images/ReadImageFile.h>
#include <GL/GLGeometryWrappers.h>

// ToolBox includes
//
#include "ToolBox/ToolBox"
#include "ToolBox/Extensions/ToolRotator.h"

#include "FieldViewer.h"
#include "Tools/DotSpreaderTool.h"
#include "Tools/DynamicSolverTool.h"
#include "Tools/ParticleSprayerTool.h"
#include "Tools/StaticSolverTool.h"

#include "Directory.h"

ExperimentFactory Factory;


//#define FONT_SIZE 16.0
//#define FONT_MODIFIER 0.04

static const float FONT_SIZE=96.0;
static const float FONT_MODIFIER=0.006;

/** Returns the base directory for resource files.
 *
 * Returns RESOURCEDIR if it exists or
 * returns "." to search the current directory instead.
 */
std::string getResourceDir()
{
    std::string dir(RESOURCEDIR);

    try {
        IO::DirectoryPtr dirPtr = IO::openDirectory(dir.c_str());
    }
    catch (IO::Directory::OpenError e)
    {
        // This means FieldViewer must exist in the CWD.
        dir = ".";
    }
    return dir;
}

Vrui::Scalar getAngle(const Vrui::Vector& u, const Vrui::Vector& v)
{
    return std::acos((u * v) / (Geometry::mag(u) * Geometry::mag(v)));
}


Viewer::Viewer(int &argc, char** argv, char** appDefaults) :
   Vrui::Application(argc, argv, appDefaults),
   tools(ToolList()),
   experiment(NULL),
   frameRateDialog(NULL),
   positionDialog(NULL),
   experimentDialog(NULL),
   currentOptionsDialog(NULL),
   optionsDialogs(DialogArray()),
   toolbox(0),
   elapsedTime(0.0),
   absoluteTime(0.0),
   masterout(std::cout), nodeout(std::cout), debugout(std::cerr),
   showingLogo(false),
   firstTime(true),
   startLogo(true)
{

    // load ToolBox
    ToolBox::ToolBoxFactory::instance();

    // load dynamics plugins
    try
    {
        experiment_names = loadPlugins();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "UNDEFINED ERROR." << std::endl;
    }

    // alphabetize the names list
    std::sort(experiment_names.begin(), experiment_names.end());

    // create and set the main menu
    mainMenu=createMainMenu();
    Vrui::setMainMenu(mainMenu);

    // create other dialogs
    frameRateDialog = new FrameRateDialog(mainMenu);
    positionDialog = new PositionDialog(mainMenu);

    /** Prepare to show logo! **/
    setExperiment("Lorenz", false);
    //setExperiment("Lorenz", false); // find out why we must do this twice in order to avoid flicker
	//beginLogo();
}

Viewer::~Viewer()
{
    delete mainMenu;

    if (experimentDialog != NULL) delete experimentDialog;

    if (experiment != NULL) delete experiment;

    for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
    {
        delete *tool;
    }

    // close all dynamic libs (plugins)
    for (DLList::iterator lib=dl_list.begin(); lib != dl_list.end(); ++lib)
    {
        dlclose(*lib);
    }
}

//
// Internal methods
//

bool Viewer::loadViewpointFile(IO::Directory& directory,const char* viewpointFileName)
{
const char* vruiViewpointFileHeader="Vrui viewpoint file v1.0\n";
bool result=false;

try
	{
	/* Open the viewpoint file: */
	IO::FilePtr viewpointFile=directory.openFile(viewpointFileName);
	viewpointFile->setEndianness(Misc::LittleEndian);

	/* Check the header: */
	char header[80];
	viewpointFile->read(header,strlen(vruiViewpointFileHeader));
	header[strlen(vruiViewpointFileHeader)]='\0';
	if(strcmp(header,vruiViewpointFileHeader)==0)
		{
		/* Read the environment's center point in navigational coordinates: */
		Vrui::Point center;
		viewpointFile->read<Scalar>(center.getComponents(),3);

		/* Read the environment's size in navigational coordinates: */
		Scalar size=viewpointFile->read<Scalar>();

		/* Read the environment's forward direction in navigational coordinates: */
		Vrui::Vector forward;
		viewpointFile->read<Scalar>(forward.getComponents(),3);

		/* Read the environment's up direction in navigational coordinates: */
		Vrui::Vector up;
		viewpointFile->read<Scalar>(up.getComponents(),3);

		/* Construct the navigation transformation: */
		Vrui::NavTransform nav=Vrui::NavTransform::identity;
		nav*=Vrui::NavTransform::translateFromOriginTo(Vrui::getDisplayCenter());
		nav*=Vrui::NavTransform::rotate(Vrui::Rotation::fromBaseVectors(Geometry::cross(Vrui::getForwardDirection(),Vrui::getUpDirection()),Vrui::getForwardDirection()));
		nav*=Vrui::NavTransform::scale(Vrui::getDisplaySize()/size);
		nav*=Vrui::NavTransform::rotate(Geometry::invert(Vrui::Rotation::fromBaseVectors(Geometry::cross(forward,up),forward)));
		nav*=Vrui::NavTransform::translateToOriginFrom(center);
		Vrui::setNavigationTransformation(nav);

		result=true;
		}
	}
catch(std::runtime_error error)
	{
	/* Ignore the error and return a failure code: */
	}

return result;
}

void Viewer::drawLogo(GLContextData& contextData) const
{

    DTS::DataItem* dataItem=contextData.retrieveDataItem<DTS::DataItem> (this);

   	// Haven't figure out what Render() is changing...but unless we push
    // GL_TEXTURE_BIT, the rendered text disappears on the second call to
    // display() on some platforms (eg Linux on Intel Mac).
    //glPushAttrib(GL_TEXTURE_BIT);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // <-- Don't forget this!
    glLoadMatrix(Vrui::getDisplayState(contextData).modelviewPhysical);

    Vrui::Point center=Vrui::getDisplayCenter();
    glTranslate(center-Vrui::Point::origin); // Now origin is in center of environment

    Vrui::Vector fw=Vrui::getForwardDirection();
    Vrui::Vector up=Vrui::getUpDirection();
    Vrui::Vector right=Geometry::cross(fw,up);
    glRotate(Vrui::Rotation::fromBaseVectors(right,up)); // Now x goes right, y goes up

    Vrui::Scalar ds=Vrui::getDisplaySize();
    glScale(ds,ds,ds); // Now radius 1 corresponds to environment size

    glScalef(FONT_MODIFIER, FONT_MODIFIER, FONT_MODIFIER);
    glTranslatef(-400, 150, 0);
    if (absoluteTime > 3.5)
	{
        glColor3f(1.0,1.0,1.0);
    	dataItem->font->Render("flow.");
    }
    glPopMatrix(); // <-- Don't forget this!

    glDisable(GL_BLEND);
    glPopAttrib();
}

void Viewer::stepLogo()
{
    if (experiment == NULL || toolbox == NULL)
    {
        return;
    }

    /* requires an experiment */

    if (firstTime)
    {
        /* Spread some particles */
        experiment->integrator->setRealParamValue("stepSize", .01);
        std::map<std::string, AbstractDynamicsTool*>::iterator it = toolmap.find("DotSpreaderTool");
        if (it != toolmap.end())
        {
           DTS::Vector<double> position = experiment->transformer->getCenterPoint();
           Vrui::Point pos;
           pos[0] = position[0];
           pos[1] = position[1];
           pos[2] = position[2];
           DotSpreaderTool* tool = static_cast<DotSpreaderTool*>(it->second);
           tool->setPointSize(.25);
           tool->releaseParticles(pos, experiment->transformer->getRadius());
        }

        firstTime = false;

		// load viewPoint file transformation.
		std::string dirstr( getResourceDir() + "/views");
		IO::StandardDirectory dir(dirstr);
		std::string resource = "logo.view";
		loadViewpointFile(dir, resource.c_str());

    }

	double oldValue = experiment->integrator->getRealParamValue("stepSize");
	double newValue = .9999 * oldValue;
	if (newValue < .0001)
	{
	    newValue = .0001;
	}
	experiment->integrator->setRealParamValue("stepSize", newValue);
}

void Viewer::setRadioToggles(ToggleArray& toggles, const std::string& name)
{
   for (ToggleArray::iterator button=toggles.begin(); button != toggles.end(); ++button) {
      if (strcmp((*button)->getName(), name.c_str()) != 0
            and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);
   }
}

std::vector<std::string> Viewer::loadPlugins() throw(std::runtime_error)
{
    std::string directory( getResourceDir() );
    directory += "/plugins";

    std::cout << "Loading plugins from: " << directory << std::endl;

    Directory dir;
    dir.addExtensionFilter("so");
    dir.read(directory);

    void *dlib;
    std::vector<std::string>::const_iterator lib;
    for (lib=dir.contents().begin(); lib != dir.contents().end(); ++lib)
    {
        std::cout << "\tOpening " << *lib << "..." << std::endl;

        // prepend directory name to library file name
        std::string file=directory + "/" + *lib;

        // open the library
        dlib=dlopen(file.c_str(), RTLD_NOW);
        //dlib=dlopen(file.c_str(), RTLD_LAZY|RTLD_GLOBAL);

        if (dlib == NULL)
        {
            throw std::runtime_error(dlerror());
        }

        // add the handle to our list
        dl_list.insert(dl_list.end(), dlib);
    }

    // create an array of model names
    std::vector<std::string> experiment_names;
    ExperimentFactory::iterator itr;

    for (itr = Factory.begin(); itr != Factory.end(); ++itr)
    {
        experiment_names.insert(experiment_names.end(), itr->first);
    }

    // return the name array
    return experiment_names;
}


//
// Interface
//

void Viewer::initContext(GLContextData& contextData) const
{
   // create data item and add it to the context data
   DTS::DataItem* dataItem=new DTS::DataItem;
   contextData.addDataItem(this, dataItem);

   std::string directory(getResourceDir());

   // load the font
   dataItem->font = new FTGLTextureFont((directory+"/fonts/VeggiMed.otf").c_str());
   dataItem->font->FaceSize(FONT_SIZE);

   // get point sprite image
   Images::RGBImage spriteTexture=Images::readImageFile((directory+"/images/particle.png").c_str());

   // initialize texture parameters
   glBindTexture(GL_TEXTURE_2D, dataItem->spriteTextureObjectId);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   spriteTexture.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB);

   glBindTexture(GL_TEXTURE_2D, 0);
}

void Viewer::display(GLContextData& contextData) const
{
    if(showingLogo)
    {
        drawLogo(contextData);
    }

   // get data item from context data
   DTS::DataItem* dataItem=contextData.retrieveDataItem<DTS::DataItem> (this);

   renderTools(dataItem);
}

void Viewer::renderTools(DTS::DataItem* dataItem) const
{
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
   // frame rate
   double frameTime = Vrui::getCurrentFrameTime();
   double throttledFrameRate = frameRateDialog->getThrottledFrameRate();
   frameRateDialog->setFrameRate(1.0/frameTime);
   elapsedTime += frameTime;
   absoluteTime += frameTime;

   bool stepTools = false;
   if (elapsedTime >= 1.0/throttledFrameRate)
   {
      stepTools = true;
      //std::cout << "throttled value reached" << std::endl;
      elapsedTime = 0.0;
   }

    if(experiment == NULL)
    {
        if (!showingLogo)
        {
			beginLogo();
		}

        // With the dummy navigation tool enabled, the navigation tool
        // tied to the device is blocked.  This other navigation tool
        // was actively updating the navigation transformation whenever
        // the window containing Vrui moved.  Since we are blocking this
        // tool, we must manually reset the navigation transformation.
        Vrui::setNavigationTransformation(Vrui::Point::origin, 30);
        // update the rotating tetrahedron in the logo
        Vrui::scheduleUpdate(Vrui::getApplicationTime()+0.02);
	}

	if (showingLogo)
	{
		stepLogo();
	}

    // If the experiment is not NULL, then we still might want dynamics
    // while the logo is running.  So we do not exit out just because
    // the logo is running. We exit only if there is no experiment.
    if (experiment == NULL)
    {
        return;
    }

   bool updatedExperiment = false;
   if ( experiment->isOutdated() )
   {
       updatedExperiment = true;
       experiment->updateVersion();
   }

   // iterate over all tools and do required processing
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
        /* Update position whether the tool is disabled or not */
        // If we want multiple users to be able to track various tools,
        // we'll need to go back to the TrackerTool creation. For single
        // position tracking, we just track the position of the first tool.
        if (tool == tools.begin())
        {
            //Vrui::getDeviceTransformation(input.getDevice(0)).getOrigin();
            Vrui::Point position = tools[0]->toolBox()->deviceTransformationInModel().getOrigin();
            positionDialog->setPosition(position);
        }

        if ((*tool)->isDisabled())
        {
            continue;
        }
        else
        {
            if (updatedExperiment)
            {
              (*tool)->updatedExperiment();
            }

            if (stepTools)
            {
             (*tool)->step();
            }
        }
    }

    if (startLogo && !showingLogo)
    {
        /* Need to figure this out. We cannot start spreading dots until
         * the DotSpreader's frame() function has run at least once.  Maybe
         * this is related to the DotSpreaders initContext not being ready
         * by the time we start the dot spreading.
         */
        beginLogo();
        startLogo = false;
    }

    // as quickly as possible
    Vrui::requestUpdate();
}

void Viewer::beginLogo()
{
	showingLogo = true;

	/* disable user navigation with a dummy navigation tool */
	Vrui::activateNavigationTool(reinterpret_cast<Vrui::Tool*>(this));


   // Unlock all tools (in case we are coming from the logo)
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
     (*tool)->setLocked(true);
   }
}

void Viewer::endLogo()
{
	// re-enable resetNavigationCallbacks
	showingLogo = false;

	// renable user navigation
	Vrui::deactivateNavigationTool(reinterpret_cast<Vrui::Tool*>(this));

   // Unlock all tools (in case we are coming from the logo)
   for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
   {
     (*tool)->setLocked(false);
   }
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
      if (tool != 0)
      {
          bool state=tool->isDisabled();
          (*button)->setToggle(!state);
      }
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
      if (*tool == currentTool) break;
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
   {
      currentOptionsDialog->show();
   }
}

//
// Callbacks
//

void Viewer::toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData)
{
   ToolBox::ToolBox* toolBox=dynamic_cast<ToolBox::ToolBox*> (cbData->tool);

   if (toolBox == 0) return; // bad cast, not a ToolBox

   if (toolbox == 0) // If we don't already have a ToolBox
   {
      // store it
      toolbox = toolBox;

      (new ToolBox::Extensions::ToolRotator(toolBox))->alwaysShowCurrent(false).positionOfCurrent(180).closeDelay(2.0);

      std::cout << "Creating tools and adding to toolbox..." << std::endl;

      AbstractDynamicsTool *tool;

      // add tools

      std::cout << "\tAdding Static Solver..." << std::endl;

      tool=new StaticSolverTool(toolBox, this);
      if (experiment != NULL) tool->setExperiment(experiment);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["StaticSolverTool"]=tool;

      masterout() << "\tAdding Dot Spreader..." << std::endl;

      tool=new DotSpreaderTool(toolBox, this);
      if (experiment != NULL) tool->setExperiment(experiment);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["DotSpreaderTool"]=tool;

      masterout() << "\tAdding Particle Sprayer..." << std::endl;

      tool=new ParticleSprayerTool(toolBox, this);
      if (experiment != NULL) tool->setExperiment(experiment);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["ParticleSprayerTool"]=tool;

      masterout() << "\tAdding Dynamic Solver..." << std::endl;

      tool=new DynamicSolverTool(toolBox, this);
      if (experiment != NULL) tool->setExperiment(experiment);
      tools.push_back(tool);
      // create associated options dialog and add to dialog array
      optionsDialogs.push_back(tool->createOptionsDialog(mainMenu));

      toolmap["DynamicSolverTool"]=tool;

      // automatically load the first tool and set options dialog
      AbstractDynamicsTool* currentTool = static_cast<AbstractDynamicsTool*>(tools.front());
      currentTool->grab();

      updateCurrentOptionsDialog();
      updateToolToggles();

      if (showingLogo)
      {
          for (ToolList::iterator tool=tools.begin(); tool != tools.end(); ++tool)
          {
              (*tool)->setLocked(true);
          }
      }

   }
   else
   {
        std::cerr << "ERROR: Multiple ToolRotators are not supported." << std::endl;
   }
}

void Viewer::toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData)
{
   ToolBox::ToolBox* toolBox=dynamic_cast<ToolBox::ToolBox*> (cbData->tool);
   if (toolBox != 0 && toolBox == toolbox)
   {
      // need to fix this to handle multiple users each with their own toolbox
      tools.clear();
      toolmap.clear();

      optionsDialogs.clear();
      currentOptionsDialog = NULL;
   }
}

void Viewer::resetNavigationCallback(Misc::CallbackData* cbData)
{
    if (showingLogo)
    {
        // ignore this request
        return;
    }

    resetView();
}

void Viewer::resetView()
{
    if (experiment != NULL)
    {
        // if experiment defines default view, go there.
        // otherwise, go to center.
        DTS::Vector<double> center = experiment->transformer->getCenterPoint();
        Vrui::Point p;
        p[0] = center[0];
        p[1] = center[1];
        p[2] = center[2];
        double radius = experiment->transformer->getRadius();
        Vrui::setNavigationTransformation(p, radius);
    }
}

void Viewer::mainMenuTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData *cbData)
{
   std::string name=cbData->toggle->getName();

   if (name == "ShowParameterDialogToggle")
   {
      // if toggle is set show the parameter dialog
      if (cbData->toggle->getToggle())
      {
         if (!experimentDialog || showingLogo)
         {
            cbData->toggle->setToggle(false);
            return;
         }
         experimentDialog->show();
      }
      // otherwise hide the parameter dialog
      else
      {
         experimentDialog->hide();
      }
   }
   else if (name == "ShowOptionsDialogsToggle")
   {
      // if toggle is set show tool options dialogs
      if (cbData->toggle->getToggle())
      {
         // if no tool is set reset toggle and return
    if (!currentOptionsDialog || showingLogo)
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
	std::string name = cbData->toggle->getName();
	std::string key(name);
	key.erase( key.find("toggle") );

	setExperiment(key);

	// Stop splash screen mode and unlock all the tools.
	endLogo();

	// Load the initial static solution
	std::map<std::string, AbstractDynamicsTool*>::iterator it;


    it = toolmap.find("StaticSolverTool");
	if (it != toolmap.end())
	{
		StaticSolverTool* tool = static_cast<StaticSolverTool*>(it->second);
		tool->addStaticSolution(experiment->transformer->getDefaultPoint());
	}

    /**
    it = toolmap.find("DotSpreaderTool");
    if (it != toolmap.end())
    {
       DTS::Vector<double> position = experiment->transformer->getCenterPoint();
       Vrui::Point pos;
       pos[0] = position[0];
       pos[1] = position[1];
       pos[2] = position[2];
       DotSpreaderTool* tool = static_cast<DotSpreaderTool*>(it->second);
       tool->releaseParticles(pos, experiment->transformer->getRadius());
    }
    **/

   // Center the model
   resetView();
}


void Viewer::setExperiment(std::string name, bool updateToggle)
{
   bool popup=false;

   // delete current dynamical model
   if (experiment != NULL)
      delete experiment;

   GLMotif::WidgetManager::Transformation oldTrans;

   // delete current parameter dialog
   bool dialogExisted = false;
   if (experimentDialog != NULL)
   {
      // save the old transformation
      dialogExisted = true;
      oldTrans=experimentDialog->getTransformation();

      // if dialog is currently shown hide before deleting
      if (experimentDialog->state() == CaveDialog::ACTIVE)
      {
         experimentDialog->hide();

         // set flag to popup after creating new dialog
         popup=true;
      }

      delete experimentDialog;
   }

   // create the dynamical model
   experiment = Factory[name]();

   // create/assign parameter dialog
   experimentDialog = new ExperimentDialog(mainMenu, experiment);
   if (dialogExisted)
   {
        experimentDialog->setTransformation(oldTrans);
   }

   // popup parameter dialog if previously shown or system requests it
   if (popup)
      experimentDialog->show();

   // iterate through tools and sets experiment
   for (ToolList::iterator toolItr=tools.begin(); toolItr != tools.end(); ++toolItr)
   {
      (*toolItr)->setExperiment(experiment);
   }

   // fake radio-button behavior
   if (updateToggle)
       setRadioToggles(dynamicsToggleButtons, name + "toggle");
}

void Viewer::toolsMenuCallback(GLMotif::ToggleButton::ValueChangedCallbackData *cbData)
{
   AbstractDynamicsTool* tool;

   // If we are showing the logo, we ignore their request and revert the toggle.
   // Similarly if there is no ToolBox.

  std::string name=cbData->toggle->getName();
  if (name == "ParticleSprayerToggle")
  {

     if (showingLogo || toolbox == 0)
     {
        cbData->toggle->setToggle( !cbData->toggle->getToggle() );
     }
     else
     {
         //masterout() << "Current Tool: Particle Sprayer" << std::endl;
         tool=toolmap["ParticleSprayerTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
     }
  }
  else if (name == "DotSpreaderToggle")
  {
     if (showingLogo || toolbox == 0)
     {
        cbData->toggle->setToggle( !cbData->toggle->getToggle() );
     }
     else
     {
         //masterout() << "Current Tool: Dot Spreader" << std::endl;
         tool=toolmap["DotSpreaderTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
     }
  }
  else if (name == "StaticSolverToggle")
  {
     if (showingLogo || toolbox == 0)
     {
        cbData->toggle->setToggle( !cbData->toggle->getToggle() );
     }
     else
     {
         //masterout() << "Current Tool: Static Solver" << std::endl;
         tool=toolmap["StaticSolverTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
     }
  }
  else if (name == "DynamicSolverToggle")
  {

     if (showingLogo || toolbox == 0)
     {
        cbData->toggle->setToggle( !cbData->toggle->getToggle() );
     }
     else
     {
         //masterout() << "Current Tool: Dynamic Solver" << std::endl;
         tool=toolmap["DynamicSolverTool"];
         bool state=tool->isDisabled();
         tool->setDisabled(!state);
     }
  }
  else
  {
  }
}

