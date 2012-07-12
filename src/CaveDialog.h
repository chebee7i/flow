/*******************************************************************************
 CaveDialog: Abstract base class for Vrui dialogs.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#ifndef CAVE_DIALOG_H
#define CAVE_DIALOG_H

// Vrui includes
//
#include <Vrui/Vrui.h>
#include <GLMotif/WidgetManager.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/PopupMenu.h>

/** Abstract base class for Vrui dialogs.
 *
 * The CaveDialog class attempts to simplify the process of managing a
 * Vrui dialog as well as providing some additional features (for example
 * a CaveDialog object will remember its previous location and "pop up"
 * in the same place.
 *
 * \note Sub-classes of CaveDialog must include the following code in
 * their constructor;
 * \code
 *   dialogWindow = createDialog();
 * \endcode
 */
class CaveDialog
{
   public:
      enum State
      {
         ACTIVE, HIDDEN
      };

      CaveDialog(GLMotif::PopupMenu *parentMenu) :
         parent(parentMenu), dialogWindow(0), CurrentState(HIDDEN),
         isCurrent(false)
      {
      }

      virtual ~CaveDialog()
      {
         if (dialogWindow)
            delete dialogWindow;
      }

      /** Return the current state of the dialog.
       */
      State state() const
      {
         return CurrentState;
      }

      /** Display (pop-up) the dialog.
       *
       * If the dialog has a saved state it will pop-up in its previous position.
       * Otherwise, it will popup in the position of its parent, which is
       * typically the application's main menu.
       */
      void show()
      {
         if (!isCurrent)
         {
            transformation=Vrui::getWidgetManager()->calcWidgetTransformation(parent);
            isCurrent=true;
         }

         Vrui::getWidgetManager()->popupPrimaryWidget(dialogWindow, transformation);
         CurrentState=ACTIVE;
      }

      /** Hide the dialog.
       *
       * The current position is saved for the next time it is shown.
       */
      void hide()
      {
         transformation=Vrui::getWidgetManager()->calcWidgetTransformation(dialogWindow);

         Vrui::popdownPrimaryWidget(dialogWindow);
         CurrentState=HIDDEN;
      }

      /** Return the saved transformation (position).
       *
       * This method is useful for swapping out one dialog for another. By using
       * the transformation of the old dialog, the new dialog can be placed in the
       * exact same position.
       *
       * \note If the dialog has no saved state (has never been shown) it will
       * return the transformation of its parent, which is typically the application's
       * main menu.
       */
      GLMotif::WidgetManager::Transformation getTransformation() const
      {
         if (CurrentState == HIDDEN)
         {
            if (isCurrent)
            {
               // return the last saved position
               return transformation;
            }
            else
            {
               // return location of parent widget
               return Vrui::getWidgetManager()->calcWidgetTransformation(parent);
            }
         }
         else
         {
            return Vrui::getWidgetManager()->calcWidgetTransformation(dialogWindow);
         }
      }

      /** Set the transformation (position) of the dialog.
       */
      void setTransformation(const GLMotif::WidgetManager::Transformation& trans)
      {
         transformation=trans;
         isCurrent=true;
      }

      bool initialized() const
      {
         return isCurrent;
      }

      void reset()
      {
         isCurrent=false;
      }

   protected:
      virtual GLMotif::PopupWindow* createDialog() = 0;

      GLMotif::PopupMenu *parent; ///< The dialog's parent menu.
      GLMotif::PopupWindow *dialogWindow; ///< The actual dialog window.
      State CurrentState; ///< Whether the dialog is currently shown (popped-up) or hidden.

      bool isCurrent; ///< Flag whether the dialog's transformation is valid.
      GLMotif::WidgetManager::Transformation transformation; ///< Previously saved position.
};

#endif
