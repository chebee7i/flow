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

#ifndef TOOLBOX_EVENTS_H
#define TOOLBOX_EVENTS_H

//#include <Misc/CallbackData.h>
//#include <Vrui/Geometry.h>

namespace ToolBox {
	class ToolBox ;
	class Tool ;
}

namespace ToolBox {

typedef std::pair < int, int > ButtonId ;

//class Event : public Misc::CallbackData
class Event
{
	/* (De)Constructors: */
	public:
	Event ( ToolBox* toolBox, Tool* tool )
		: toolBox ( toolBox ) ,
		  tool ( tool )
	{ }
	virtual ~Event ( ) { }

	/* Elements: */
	public:
	ToolBox * toolBox ; // The toolbox causing the event
	Tool * tool ; // The subject tool
} ;

class ToolGrabEvent : public Event
{
	public:
	ToolGrabEvent ( ToolBox* toolBox, Tool* tool )
		: Event ( toolBox, tool )
	{ }
} ;

class ToolDropEvent : public Event
{
	public:
	ToolDropEvent ( ToolBox* toolBox, Tool* tool )
		: Event ( toolBox, tool )
	{ }
} ;

class ToolAddEvent : public Event
{
	public:
	ToolAddEvent ( ToolBox* toolBox, Tool* tool )
		: Event ( toolBox, tool )
	{ }
} ;

class ToolRemoveEvent : public Event
{
	public:
	ToolRemoveEvent ( ToolBox* toolBox, Tool* tool )
		: Event ( toolBox, tool )
	{ }
} ;

class MotionEvent : public Event
{
	public:
	MotionEvent ( ToolBox* toolBox, Tool* tool )
		: Event ( toolBox, tool )
	{ }
} ;

class ButtonEvent : public Event
{
	public:
	ButtonEvent ( ToolBox* toolBox, Tool* tool, ButtonId button, bool state )
		: Event ( toolBox, tool ) , button ( button ), state ( state )
	{ }

	public:
	ButtonId button ;
	bool state ;
} ;

class ButtonPressEvent : public ButtonEvent
{
	public:
	ButtonPressEvent ( ToolBox* toolBox, Tool* tool, ButtonId button )
		: ButtonEvent ( toolBox, tool, button, true )
	{ }
} ;

class ButtonReleaseEvent : public ButtonEvent
{
	public:
	ButtonReleaseEvent ( ToolBox* toolBox, Tool* tool, ButtonId button )
		: ButtonEvent ( toolBox, tool, button, false )
	{ }
} ;

} // namespace ToolBox

#endif
