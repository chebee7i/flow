/*******************************************************************************
Copyright (c) 2006-2008 Braden Pellett

This file is part of the Tool Box Library.

The Tool Box Library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or (at your option) any 
later version.

The Tool Box Library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.

You should have received a copy of the GNU General Public License
along with the Tool Box Library. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef TOOLBOX_TOOLS_VRUIINPUTDEVICE_H
#define TOOLBOX_TOOLS_VRUIINPUTDEVICE_H

#include <ToolBox/Tool.h>

#include <string>

namespace Vrui {
	class InputDevice ;
}

namespace ToolBox {
namespace Tools {

class VruiInputDevice : public Tool
{
	/* (De)Constructors */
	public:
	VruiInputDevice ( ToolBox * toolBox, const std::string & name ) ;
	~VruiInputDevice ( ) ;

	/* Event Callbacks */
	public:
	void moved ( const MotionEvent & motionEvent ) ;
	void mainButtonPressed ( const ButtonPressEvent & buttonPressEvent ) ;
	void mainButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) ;
	void otherButtonPressed ( const ButtonPressEvent & buttonPressEvent ) ;
	void otherButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) ;
	void grabbed ( const ToolGrabEvent & toolGrabEvent ) ;
	void dropped ( const ToolDropEvent & toolDropEvent ) ;

	public:
	Vrui::InputDevice* mVruiInputDevice ;
} ;

} // namespace Tools
} // namespace ToolBox

#endif
