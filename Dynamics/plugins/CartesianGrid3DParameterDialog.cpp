#include <stdio.h>
#include <iostream>
#include <string>

#include <IO/OpenFile.h>
 
#include "RungeKutta4.h"
#include "boost/multi_array.hpp"
#include "CartesianGrid3D.h"

#include "CartesianGrid3DParameterDialog.h"
#include "GLMotif/WidgetFactory.h"
#include "IntegrationStepSize.h"
#include "VruiStreamManip.h"

GLMotif::PopupWindow* CartesianGrid3DParameterDialog::createDialog()
{
  WidgetFactory factory;
  GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", "CartesianGrid3D Parameters");

  GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 3);
  factory.setLayout(parameterDialog);
   
  openFileButton = factory.createButton("OpenFileButton", "Load Data");
  openFileStatusA = factory.createLabel("OpenFileStatusA", "");
  openFileStatusB = factory.createLabel("OpenFileStatusB", "");
  openFileButton->getSelectCallbacks().add(this, &CartesianGrid3DParameterDialog::openFileCallback);

  char cwd[512];
  getcwd(cwd, 512);
  std::string dataDirectory(cwd);
  IO::DirectoryPtr dirptr = IO::openDirectory(dataDirectory.c_str());

  fileDialog = new GLMotif::FileSelectionDialog(Vrui::getWidgetManager(), "Open file...", dirptr, ".grid");
  // Need to modify FileSelectionDialog to accept filterDescriptions. For ex: "Simple Grid (.grid)"
  //  fileDialog->addFileNameFilters("Numpy Array (.npy)");
  fileDialog->getOKCallbacks().add(this, &CartesianGrid3DParameterDialog::fileOpenAction);
  fileDialog->getCancelCallbacks().add(this, &CartesianGrid3DParameterDialog::fileCancelAction);

/*  factory.createLabel("Dummy3", "");
  openFileStatusC = factory.createLabel("OpenFileStatusC", "");
  factory.createLabel("Dummy4", "");
*/

  factory.createLabel("Dummy3b", "");
  shapeLabel = factory.createLabel("ShapeLabel", "");
  shapeValue = factory.createLabel("ShapeValue", "");

  factory.createLabel("Dummy5", "");
  factory.createLabel("Dummy6", "");
  factory.createLabel("Dummy7", "");

  factory.createLabel("StepSizeLabel", "step size");
  stepSizeValue=factory.createTextField("StepSizeValue", 10);
  double step_size = IntegrationStepSize::instance()->getSavedValue("CartesianGrid3D");
  if (step_size > 0.0) stepSizeValue->setString(toString(step_size).c_str());
  else stepSizeValue->setString("0.01");
  stepSizeSlider=factory.createSlider("StepSizeSlider", 15.0);
  stepSizeSlider->setValueRange(0.0001, 0.05, 0.0001);
  if (step_size > 0.0) stepSizeSlider->setValue(step_size);
  else stepSizeSlider->setValue(0.01);
  stepSizeSlider->getValueChangedCallbacks().add(this, &CartesianGrid3DParameterDialog::sliderCallback);

  parameterDialog->manageChild();
  return parameterDialogPopup;
}

void CartesianGrid3DParameterDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
  double value = cbData->value;

  char buff[10];
  snprintf(buff, sizeof(buff), "%3.2f", value);

  if (strcmp(cbData->slider->getName(), "StepSizeSlider")==0)
  {
    snprintf(buff, sizeof(buff), "%6.4f", value);
    stepSizeValue->setString(buff);
    IntegrationStepSize::instance()->setValue(value);
    IntegrationStepSize::instance()->saveValue("CartesianGrid3D", value);
  }
}

void CartesianGrid3DParameterDialog::openFileCallback(Misc::CallbackData* cbData)
{
	Vrui::getWidgetManager()->popupPrimaryWidget(fileDialog, 
                                                 Vrui::getWidgetManager()->calcWidgetTransformation(dialogWindow));
}

void CartesianGrid3DParameterDialog::fileOpenAction(GLMotif::FileSelectionDialog::OKCallbackData* cbData) 
{
    
    // hide the dialog
    Vrui::popdownPrimaryWidget(fileDialog);

    // status information
    std::string message("Loading...");
    openFileStatusA->setString(message.c_str());

    // When exiting Vrui, was getting an error. So we make an explicit copy.
    filename = std::string(cbData->selectedFileName);
    openFileStatusB->setString(filename.c_str());

    openFile();
}

void CartesianGrid3DParameterDialog::fileCancelAction(GLMotif::FileSelectionDialog::CancelCallbackData*) 
{
    Vrui::popdownPrimaryWidget(fileDialog);
}

void CartesianGrid3DParameterDialog::openFile()
{
    size_t found = filename.rfind(".");
    std::string ext = "";
    if (found != std::string::npos)
    {
        ext = filename.substr(found+1);
    }

    if (ext == "grid")
    {
        SimpleGridReader(filename, this->model);
        shapeLabel->setString("Shape:");
        std::ostringstream os;
        os << "(" << model->dynamics->getSpacing(0) << ", " 
                  << model->dynamics->getSpacing(1) << ", "
                  << model->dynamics->getSpacing(2) << ")";
        std::string s(os.str());
        shapeValue->setString(s.c_str());
    }
    else
    {
        std::cout << "Unknown filetype: ." << ext << std::endl;
    }

    // status update
    std::string message("File:");
    openFileStatusA->setString(message.c_str());
}

SimpleGridReader::SimpleGridReader(std::string filename, CartesianGrid3DIntegrator* m) 
    : filename(filename), model(m) 
{

    typedef CartesianGrid3D::Point Point;
    typedef CartesianGrid3D::Scalar Scalar;
    typedef CartesianGrid3D::Vector Vector;

    FILE* file=fopen(filename.c_str(), "rt");

    // initialize memory
    Point gridSpacing(1,1,1);
    Point firstPosition(0,0,0);
    std::vector<int> gridShape(3,1); // fill with ones

    // shape is first line, all integers
    fscanf(file, "%d %d %d", &gridShape[0], &gridShape[1], &gridShape[2]);
    std::cout << gridShape[0] << " " << gridShape[1] << " " << gridShape[2] << std::endl;

    boost::array<CartesianGrid3D::index, 3> shape = {{ 200, 200, 200 }};
    model->grid = new CartesianGrid3D::grid_type(shape);


//      Point gridSpacing(1,1,1);
//      Point firstPosition(-50,-50,-50);

      /** lorentz **/

      typedef CartesianGrid3D::index index;
      Scalar x,y,z;
      Scalar sigma = 10;
      Scalar r = 21;
      Scalar b = 2.6;
      for(index i = 0; i != shape[0]; ++i) 
      {
        x = firstPosition[0] + gridSpacing[0] * i;
        for(index j = 0; j != shape[1]; ++j)
        {
          y = firstPosition[1] + gridSpacing[1] * j;
          for(index k = 0; k != shape[2]; ++k)
          {
            z = firstPosition[2] + gridSpacing[2] * k;
            (*model->grid)[i][j][k] = Vector(sigma*(y-x), r*x-y-x*z, x*y-b*z);
          }
        }
      }  

    fclose(file);

      model->dynamics = new CartesianGrid3D(*model->grid, gridSpacing, firstPosition);
      double stepSize = 0.01;
      model->integrator = new RungeKutta4<CartesianGrid3D>(*(model->dynamics), stepSize);
}

