#ifndef POSITIONDIALOG_H_
#define POSITIONDIALOG_H_

#include <GLMotif/GLMotif>
#include "CaveDialog.h"

class PositionDialog : public CaveDialog
{
private:
  GLMotif::TextField *x;
  GLMotif::TextField *y;
  GLMotif::TextField *z;

protected:
  GLMotif::PopupWindow* createDialog();

public:
  PositionDialog(GLMotif::PopupMenu *parentMenu)
     : CaveDialog(parentMenu)
  {
    dialogWindow=createDialog();
  }

  virtual ~PositionDialog() { }

  // Methods to set the labels.
  void setPosition(Vrui::Point position);
  void setX(double xvalue);
  void setY(double yvalue);
  void setZ(double zvalue);
};

#endif

