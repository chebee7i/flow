
#ifndef CARTESIANGRID3DPARAMETERDIALOG_H_
#define CARTESIANGRID3DPARAMETERDIALOG_H_

#include <GLMotif/GLMotif>
#include <GLMotif/FileSelectionDialog.h>

//#include "BaseModel.h"
#include "CaveDialog.h"

// forward declaration
class CartesianGrid3DIntegrator;

class CartesianGrid3DParameterDialog : public CaveDialog
{
private:
  CartesianGrid3DIntegrator* model;

  GLMotif::Button       *openFileButton;
  GLMotif::Label        *openFileStatusA;
  GLMotif::Label        *openFileStatusB;
  GLMotif::Label        *openFileStatusC;
  GLMotif::Slider       *stepSizeSlider;
  GLMotif::TextField    *stepSizeValue;

  GLMotif::Label *shapeLabel;
  GLMotif::Label *shapeValue;

  // owned by dialog
  GLMotif::FileSelectionDialog *fileDialog;
  std::string filename;

  void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
  virtual void fileOpenAction(GLMotif::FileSelectionDialog::OKCallbackData*);
  virtual void fileCancelAction(GLMotif::FileSelectionDialog::CancelCallbackData*);
  void openFile();

protected:
  GLMotif::PopupWindow* createDialog();

public:
  CartesianGrid3DParameterDialog(GLMotif::PopupMenu *parentMenu, CartesianGrid3DIntegrator* m)
     : CaveDialog(parentMenu), model(m)
  {
    dialogWindow=createDialog();
  }

  virtual ~CartesianGrid3DParameterDialog() 
  {
    std::cout << "destructor called" << std::endl;
    delete fileDialog;
  }

  void openFileCallback(Misc::CallbackData*);
};

class SimpleGridReader
{
private:
  std::string filename;
  CartesianGrid3DIntegrator* model;  
public:
  SimpleGridReader(std::string filename, CartesianGrid3DIntegrator* m);
};
   
#endif
