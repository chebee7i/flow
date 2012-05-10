#ifndef FRAMERATEDIALOG_H_
#define FRAMERATEDIALOG_H_

#include <GLMotif/GLMotif>
#include "CaveDialog.h"

class FrameRateDialog : public CaveDialog
{
  GLMotif::Slider *throttledFrameRateSlider;
  GLMotif::TextField *currentThrottledFrameRate;
  GLMotif::TextField *currentFrameRate;

  double throttledFrameRate;

  void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);

protected:
  GLMotif::PopupWindow* createDialog();

public:
  FrameRateDialog(GLMotif::PopupMenu *parentMenu)
     : CaveDialog(parentMenu),
       throttledFrameRate(60.0)
  {
    dialogWindow=createDialog();
  }

  virtual ~FrameRateDialog() { }

  void setFrameRate(double frameRate);
  double getThrottledFrameRate();
};

#endif

