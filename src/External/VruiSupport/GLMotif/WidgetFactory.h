/*******************************************************************************
 WidgetFactory: Class for simplifying the process of creating Vrui GUI objects.
 Copyright (c) 2006-2008 Jordan Van Aalsburg
 *******************************************************************************/
#ifndef   	WIDGETFACTORY_H_
# define   	WIDGETFACTORY_H_

#include <GLMotif/GLMotif>
#include <Vrui/Vrui>

/** Creates and initializes CAVE widget objects for menus and dialogs.
 *
 *  The goal of the MenuWidgetFactory is to simplify the process of
 *  creating GLMotif objects. This is accomplished by keeping track of
 *  the relevant parent widget (layout) and relating child objects as
 *  they are created.
 */
class WidgetFactory
{
   public:

      /** Constructor.
       */
      WidgetFactory() :
         window(0), popup(0), menu(0), layout(0),
               ss(*Vrui::getWidgetManager()->getStyleSheet())
      {
      }

      /** Destructor.
       */
      ~WidgetFactory()
      {
      }

      /** Create a popup window for a menu or dialog.
       *
       *  After creating the popup window a subsequent call to
       *  createRowColumn will create a RowColumn object which is a child
       *  of the popup window. This behavior can be overridden by setting
       *  the layout explicitly using the setLayout method.
       */
      GLMotif::PopupWindow* createPopupWindow(const char* name, const char* title)
      {
         GLMotif::PopupWindow
               * popupWindow=
                     new GLMotif::PopupWindow(name, Vrui::getWidgetManager(), title, ss.font);
         window=popupWindow;
         return popupWindow;
      }

      /** Create a popup menu.
       *
       *  Creates a popup menu and initializes the various properties. Any
       *  GLMotif::Menu objects created by createMenu will automatically
       *  be designated as children of this popup menu.
       */
      GLMotif::PopupMenu* createPopupMenu(const char* name, const char* title=
            "")
      {
         GLMotif::PopupMenu* popupMenu=
               new GLMotif::PopupMenu(name, Vrui::getWidgetManager());

         popupMenu->setBorderWidth(0.0f);
         popupMenu->setBorderType(GLMotif::Widget::RAISED);
         popupMenu->setBorderColor(ss.bgColor);
         popupMenu->setBackgroundColor(ss.bgColor);
         popupMenu->setForegroundColor(ss.fgColor);
         popupMenu->setMarginWidth(ss.size);
         popupMenu->setTitleSpacing(ss.size);

         popupMenu->setTitle(title, ss.font);

         popup=popupMenu;

         return popupMenu;
      }

      /** Create a menu object.
       *
       *  Creates menu object which is a child of the popup menu created
       *  in createPopupMenu method. Will fail if PopupMenu object has not
       *  been created.
       */
      GLMotif::Menu* createMenu(const char* name)
      {
         GLMotif::Menu* mainMenu=new GLMotif::Menu(name, popup, false);

         mainMenu->setBorderWidth(0.0f);
         mainMenu->setOrientation(GLMotif::Menu::VERTICAL);
         mainMenu->setNumMinorWidgets(1);
         mainMenu->setMarginWidth(0.0f);
         mainMenu->setSpacing(ss.size);

         menu=mainMenu;

         return mainMenu;
      }

      /** Create a cascade button for sub-menus.
       *
       *  When creating a cascade button it is assumed that we are
       *  creating a menu and not a dialog. Therefore the cascade button
       *  will be made a child of the menu object created in createMenu
       *  call. This method will fail if a GLMotif::Menu object has not
       *  been created.
       */
      GLMotif::CascadeButton* createCascadeButton(const char* name, const char* title)
      {
         GLMotif::CascadeButton* cascade=
               new GLMotif::CascadeButton(name, menu, title, ss.font);
         return cascade;
      }

      /** Create a RowColumn (layout) object.
       *
       *  If a layout has been specified by the user the RowColumn object
       *  will be made a child of the current layout. If no layout has
       *  been defined (if the user is using a RowColumn object in menu
       *  instead of a GLMotif::Menu object) then the RowColum object will
       *  be made a child of the window object created in createPopupWindow.
       */
      GLMotif::RowColumn* createRowColumn(const char* name, int numWidgets)
      {
         GLMotif::RowColumn* rowColumn;
         if (layout)
            rowColumn=new GLMotif::RowColumn(name, layout, false);
         else
            rowColumn=new GLMotif::RowColumn(name, window, false);

         rowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
         rowColumn->setNumMinorWidgets(numWidgets);
         rowColumn->setMarginWidth(0.0f);
         rowColumn->setSpacing(ss.size);
         return rowColumn;
      }

      /** Create a push button.
       *
       *  The GLMotif::Button object which is created will be a child of
       *  either the GLMotif::Menu object created in createMenu or the
       *  current layout (RowColum) object set via setLayout.
       */
      GLMotif::Button* createButton(const char* name, const char* title)
      {
         GLMotif::Button* button;
         if (menu)
            button=new GLMotif::Button(name, menu, title, ss.font);
         else
            button=new GLMotif::Button(name, layout, title, ss.font);
         return button;
      }

      /** Create a toggle button.
       *
       *  Creates a toggle button in a manner similar to createButton. If
       *  a GLMotif::Menu object has been created (i.e. if we are working
       *  in a menu and not a dialog) then the button will be made a child
       *  of the main menu object. If, on the other hand, we are working
       *  on a dialog then the button will be made a child of the current
       *  layout.
       */
      GLMotif::ToggleButton* createToggleButton(const char* name, const char* title, bool state=
            false)
      {
         GLMotif::ToggleButton* toggle;
         if (menu)
            toggle=new GLMotif::ToggleButton(name, menu, title, ss.font);
         else
            toggle=new GLMotif::ToggleButton(name, layout, title, ss.font);
         toggle->setToggle(state);
         return toggle;
      }

      /** Create a check box.
       *
       *  Identical to createToggleButton except that various button
       *  properties have been set so the button resembles a check box.
       */
      GLMotif::ToggleButton* createCheckBox(const char* name, const char* title, bool state=
            false)
      {
         GLMotif::ToggleButton* toggle;
         if (menu)
            toggle=new GLMotif::ToggleButton(name, menu, title, ss.font);
         else
            toggle=new GLMotif::ToggleButton(name, layout, title, ss.font);
         toggle->setToggle(state);
         toggle->setBorderWidth(0.0f);
         toggle->setMarginWidth(0.0f);
         toggle->setHAlignment(GLFont::Left);
         return toggle;
      }

      /** Create a slider object.
       *
       *  Creates a slider object which is the child of the current
       *  layout. It is assumed that creation of the slider is taking
       *  place within a dialog and not a menu. This method will fail if
       *  the current layout has not been set by the user.
       */
      GLMotif::Slider* createSlider(const char* name, float length=3.0)
      {
         GLMotif::Slider* slider=
               new GLMotif::Slider(name, layout, GLMotif::Slider::HORIZONTAL,
               //ss.sliderHandleWidth,
               length * ss.fontHeight);

         slider->setSliderColor(ss.sliderHandleColor);
         slider->setShaftColor(ss.sliderShaftColor);
         slider->setValue(0.0);

         return slider;
      }

      /** Create a text field.
       *
       *  Creates a TextField object which is assumed to be within a
       *  dialog. The TextField will be made a child of the current
       *  layout. This method will fail if the current layout has not been
       *  set by the user.
       */
      GLMotif::TextField* createTextField(const char* name, int width)
      {
         GLMotif::TextField* textField=
               new GLMotif::TextField(name, layout, ss.font, width);
         return textField;
      }

      /** Create a text label.
       *
       *  Creates a text field which is a child of the current layout.
       */
      GLMotif::Label* createLabel(const char* name, const char* text)
      {
         return new GLMotif::Label(name, layout, text, ss.font);
      }

      /** Creates an empty text label to serve as a spacer.
       */
      GLMotif::Label* createSpacer()
      {
         return createLabel("", "");
      }

      /** Set the current layout.
       *
       *  When using the WidgetFactory within a dialog the current layout
       *  (RowColumn) object must be assigned prior to any create* method
       *  calls.
       */
      void setLayout(GLMotif::RowColumn* parent)
      {
         layout=parent;
      }

   private:
      GLMotif::PopupWindow* window;
      GLMotif::PopupMenu* popup;
      GLMotif::Menu* menu;
      GLMotif::RowColumn* layout;

      const GLMotif::StyleSheet& ss;
};

#endif 	    /* !WIDGETFACTORY_H_ */
