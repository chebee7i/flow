
#include <algorithm>

#include "ExperimentDialog.h"
#include "GLMotif/WidgetFactory.h"
#include "VruiStreamManip.h"

GLMotif::PopupWindow* ExperimentDialog::createDialog()
{
    GLMotif::TextField* textField;
    GLMotif::Slider* slider;
    
    RealParameters::iterator realItr;
    RealParameters realParams;

    IntParameters::iterator intItr;
    IntParameters intParams;


    /** Layout **/

    WidgetFactory factory;
    GLMotif::PopupWindow* popup = factory.createPopupWindow("ExperimentPopup", "Experiment Parameters");

    // main layout
    GLMotif::RowColumn* layout = factory.createRowColumn("OuterLayout", 3);
    factory.setLayout(layout);

    //
    // Model
    //

    std::string model = "Model: ";
    model.append( experiment->model->getName() );
    factory.createLabel("Model1", model.c_str());
    factory.createLabel("Model2", "");
    factory.createLabel("Model3", "");    
        
    realParams = experiment->model->getRealParams();
    for (realItr = realParams.begin(); realItr != realParams.end(); ++realItr)
    {
        factory.createLabel("", realItr->name.c_str());
        
        textField = factory.createTextField( realItr->name.c_str(), 10 );
        textField->setString( toString(realItr->value).c_str() );
        textFields.push_back( textField );
        
        slider = factory.createSlider( realItr->name.c_str(), 15);
        slider->setValueRange( realItr->minValue, realItr->maxValue, realItr->increment );
        slider->setValue( realItr->value );
        slider->getValueChangedCallbacks().add(this, &ExperimentDialog::sliderModelCallback);        
        sliders.push_back( slider );
    }


    factory.createLabel("Blank1", "");
    factory.createLabel("Blank2", "");
    factory.createLabel("Blank3", "");    

    //
    // Integrator
    //

    std::string integrator = "Integrator: ";
    integrator.append( experiment->integrator->getName() );
    factory.createLabel("Integrator1", integrator.c_str());
    factory.createLabel("Integrator2", "");
    factory.createLabel("Integrator3", "");    

    realParams = experiment->integrator->getRealParams();
    for (realItr = realParams.begin(); realItr != realParams.end(); ++realItr)
    {
        factory.createLabel("", realItr->name.c_str());
        
        textField = factory.createTextField(realItr->name.c_str(), 10);
        textField->setString(toString(realItr->value).c_str());
        textFields.push_back( textField );
        
        slider = factory.createSlider( realItr->name.c_str(), 15);
        slider->setValueRange( realItr->minValue, realItr->maxValue, realItr->increment );
        slider->setValue( realItr->value );
        slider->getValueChangedCallbacks().add(this, &ExperimentDialog::sliderIntegratorCallback);
        sliders.push_back( slider );
    }


    factory.createLabel("Blank1", "");
    factory.createLabel("Blank2", "");
    factory.createLabel("Blank3", "");    

    //
    // Transformer
    //

    std::string transformer = "Transformer: ";
    transformer.append( experiment->transformer->getName() );
    factory.createLabel("Transformer1", transformer.c_str());
    factory.createLabel("Transformer2", "");
    factory.createLabel("Transformer3", "");    

    intParams = experiment->transformer->getIntParams();
    for (intItr = intParams.begin(); intItr != intParams.end(); ++intItr)
    {
        factory.createLabel("", intItr->name.c_str());
        
        textField = factory.createTextField(intItr->name.c_str(), 10);
        textField->setString(toString(intItr->value).c_str());
        textFields.push_back( textField );
        
        slider = factory.createSlider( intItr->name.c_str(), 15);
        slider->setValueRange( intItr->minValue, intItr->maxValue, intItr->increment );
        slider->setValue( intItr->value );
        slider->getValueChangedCallbacks().add(this, &ExperimentDialog::sliderTransformerCallback);        
        sliders.push_back( slider );
    }

    layout->manageChild();
    
    return popup;
}
    
    
void ExperimentDialog::sliderModelCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
    double value = cbData->value;

    char buff[10];
    snprintf(buff, sizeof(buff), "%3.2f", value);
  
    std::vector<GLMotif::TextField *>::iterator itr; 
    for (itr = textFields.begin(); itr != textFields.end(); ++itr )
    {
        if ( strcmp( cbData->slider->getName(), (*itr)->getName() ) == 0 )
        {
            (*itr)->setString(buff);
            experiment->model->setRealParamValue(cbData->slider->getName(), value);
            break;
        }
    }
}

void ExperimentDialog::sliderIntegratorCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
    double value = cbData->value;

    char buff[10];
    snprintf(buff, sizeof(buff), "%3.3f", value);
  
    std::vector<GLMotif::TextField *>::iterator itr; 
    for (itr = textFields.begin(); itr != textFields.end(); ++itr )
    {
        if ( strcmp( cbData->slider->getName(), (*itr)->getName() ) == 0 )
        {
            (*itr)->setString(buff);
            // need to try catch here...round off errors cause failure
            experiment->integrator->setRealParamValue(cbData->slider->getName(), value);
            break;
        }
    }
}

void ExperimentDialog::sliderTransformerCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
    int value = static_cast<int>(cbData->value);

    char buff[10];
    snprintf(buff, sizeof(buff), "%1i", value);
  
    std::vector<GLMotif::TextField *>::iterator itr; 
    for (itr = textFields.begin(); itr != textFields.end(); ++itr )
    {
        if ( strcmp( cbData->slider->getName(), (*itr)->getName() ) == 0 )
        {
            (*itr)->setString(buff);
            experiment->transformer->setIntParamValue(cbData->slider->getName(), value);
            break;
        }
    }

}
    
    
