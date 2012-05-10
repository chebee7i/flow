#ifndef EXPERIMENT_DIALOG_H
#define EXPERIMENT_DIALOG_H

#include <vector>
#include <string>

#include <GLMotif/GLMotif>

#include "Dynamics/Experiment.h"
#include "CaveDialog.h"

class ExperimentDialog : public CaveDialog
{
private:
    Experiment<double>* experiment;

    std::vector<GLMotif::Slider *> sliders;
    std::vector<GLMotif::TextField *> textFields;  

    void intSliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);  
    void realSliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);

protected:
    GLMotif::PopupWindow* createDialog();

public:

    typedef ParameterClass<double>::RealParameter RealParameter;
    typedef ParameterClass<double>::IntParameter IntParameter;    

    typedef ParameterClass<double>::RealParameters RealParameters;
    typedef ParameterClass<double>::IntParameters IntParameters;    

    ExperimentDialog(GLMotif::PopupMenu *parentMenu, Experiment<double>* e)
    : CaveDialog(parentMenu), experiment(e)
    {
        dialogWindow = createDialog();
    }

    virtual ~ExperimentDialog() 
    {
    }
    
    void sliderModelCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);    
    void sliderIntegratorCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
    void sliderTransformerCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);         
};

#endif
