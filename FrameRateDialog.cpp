#include <sstream>
#include <iostream>

#include <Vrui/Vrui>

#include "FrameRateDialog.h"

#include "GLMotif/WidgetFactory.h"
#include "VruiStreamManip.h"

GLMotif::PopupWindow* FrameRateDialog::createDialog()
{
  WidgetFactory factory;
  GLMotif::PopupWindow* frameRateDialogPopup = factory.createPopupWindow("FrameRateDialogPopup", "Frame Rate Dialog");

  GLMotif::RowColumn* frameRateDialog = factory.createRowColumn("FrameRateDialog", 3);
  factory.setLayout(frameRateDialog);

  factory.createLabel("FrameRateLabel", "Vrui Frame Rate");
  currentFrameRate = factory.createTextField("CurrentFrameRate", 10);
  currentFrameRate->setString("120.0");
  factory.createLabel("DummyLabel", "");

  factory.createLabel("ThrottledFrameRateLabel", "Maximum DTS Frame Rate");
  currentThrottledFrameRate = factory.createTextField("CurrentThrottledFrameRate", 10);
  currentThrottledFrameRate->setString("60.0");
  throttledFrameRateSlider = factory.createSlider("ThrottledFrameRateSlider", 15.0);
  throttledFrameRateSlider->setValueRange(0.0, 500.0, 1.0);
  throttledFrameRateSlider->setValue(120.0);
  throttledFrameRateSlider->getValueChangedCallbacks().add(this, &FrameRateDialog::sliderCallback);

  frameRateDialog->manageChild();
  return frameRateDialogPopup;
}

void FrameRateDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
  throttledFrameRate = cbData->value;

  char buff[10];
  snprintf(buff, sizeof(buff), "%3.2f", throttledFrameRate);

  if (strcmp(cbData->slider->getName(), "ThrottledFrameRateSlider")==0)
  {
    currentThrottledFrameRate->setString(buff);
  }
}

void FrameRateDialog::setFrameRate(double frameRate)
{
  char buff[10];
  snprintf(buff, sizeof(buff), "%3.2f", frameRate);

  currentFrameRate->setString(buff);
}

double FrameRateDialog::getThrottledFrameRate()
{
  return throttledFrameRate;
}


