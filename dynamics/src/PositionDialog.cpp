#include <Vrui/Vrui>

#include "PositionDialog.h"
#include "GLMotif/WidgetFactory.h"

GLMotif::PopupWindow* PositionDialog::createDialog()
{
  WidgetFactory factory;
  GLMotif::PopupWindow* window = factory.createPopupWindow("PositionPopup", "Position");

  GLMotif::RowColumn* positionRC = factory.createRowColumn("PositionRC", 3);
  factory.setLayout(positionRC);

  //factory.createLabel("PositionLabel", "Position");
  //factory.createLabel("DummyLabel1", "");
  //factory.createLabel("DummyLabel2", "");

  x = factory.createTextField("xTextField", 12);
  y = factory.createTextField("yTextField", 12);
  z = factory.createTextField("zTextField", 12);
  
  positionRC->manageChild();
  return window;
}

void PositionDialog::setPosition(Vrui::Point position)
{
  setX(position[0]);
  setY(position[1]);
  setZ(position[2]);
}

void PositionDialog::setX(double xvalue)
{
  x->setValue(xvalue);
}

void PositionDialog::setY(double yvalue)
{
  y->setValue(yvalue);
}

void PositionDialog::setZ(double zvalue)
{
  z->setValue(zvalue);
}


