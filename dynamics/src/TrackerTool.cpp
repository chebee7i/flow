#include "FieldViewer.h"
#include "PositionDialog.h"

Viewer::TrackerToolFactory::TrackerToolFactory(Vrui::ToolManager& toolManager, Viewer* sApplication)
 : Vrui::ToolFactory("TrackerTool",toolManager), 
    application(sApplication),
    numButtons(1), 
    buttonToggleFlags(0), 
    numValuators(0)
{
    /* Initialize the input device layout of the custom tool class: */
    layout.setNumDevices(1);
    layout.setNumButtons(0,numButtons); // device 0 has 1 button
    layout.setNumValuators(0,numValuators);
	
    /* Set the custom tool class' factory pointer: */
    TrackerTool::factory=this;
}

Viewer::TrackerToolFactory::~TrackerToolFactory(void)
{
    TrackerTool::factory = 0;
    // TODO
    // need to delete all *remaining* tools created by this factory
    // otherwise, alt-f4 causes segmentation fault
    // so we need a list to keep track of created tools
}

Vrui::Tool* Viewer::TrackerToolFactory::createTool(const Vrui::ToolInputAssignment& inputAssignment) const
{
    /* Create a new object of the custom tool class: */
    TrackerTool* newTool=new TrackerTool(this,inputAssignment);
    return newTool;
}

void Viewer::TrackerToolFactory::destroyTool(Vrui::Tool* tool) const
{
    /* Cast the tool pointer to the custom tool class (not really necessary): */
    TrackerTool* trackerTool=dynamic_cast<TrackerTool*>(tool);

    /* Destroy the tool: */
    delete trackerTool;
}



/***************************************************
Static elements of class Viewer::TrackerTool:
***************************************************/

Viewer::TrackerToolFactory* Viewer::TrackerTool::factory=0;

/*******************************************
Methods of class Viewer::TrackerTool:
*******************************************/

Viewer::TrackerTool::TrackerTool(const TrackerToolFactory* factory,const Vrui::ToolInputAssignment& inputAssignment)
  :Vrui::TransformTool(factory,inputAssignment)
{
}

const Viewer::TrackerToolFactory* Viewer::TrackerTool::getFactory(void) const
{
    return factory;
}

void Viewer::TrackerTool::initialize(void) 
{
    TransformTool::initialize();
    positionDialog = new PositionDialog();
    positionDialog->show();
}

void Viewer::TrackerTool::deinitialize(void) 
{
    TransformTool::deinitialize();
    delete positionDialog;
}

void Viewer::TrackerTool::frame(void)
{
    Vrui::Point pos = Vrui::getDeviceTransformation(input.getDevice(0)).getOrigin();

    /// what IS this?  no matter what, we are always in the xz plane.
    /// on desktop...this is true whether we use a MouseTool or not.
    //transformedDevice->setTransformation(input.getDevice(0)->getTransformation());
    //Vrui::Point pos = transformedDevice->getPosition();

    positionDialog->setX(pos[0]);
    positionDialog->setY(pos[1]);
    positionDialog->setZ(pos[2]);
}







