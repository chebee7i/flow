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

#ifndef TOOLBOX_TOOL_H
#define TOOLBOX_TOOL_H

#include <list>

namespace ToolBox {
	class ToolBox ;
	class Tool ;
	class Icon ;
	class MotionEvent ;
	class ButtonPressEvent ;
	class ButtonReleaseEvent ;
	class ToolGrabEvent ;
	class ToolDropEvent ;
}

namespace ToolBox {

typedef std::list < Tool* > ToolList ;

class Tool
{
	friend class ToolBox ;

	/* (De)Constructors */
	public:
	Tool ( ToolBox* toolBox ) ;
	virtual ~Tool ( ) ;

	/* Accessors */
	public:
	ToolBox* toolBox ( ) const ;
	bool current ( ) const ;
	Icon* icon ( ) const ;

	/* Mutators */
	public:
	void icon ( Icon* icon ) ;

	/* Actions */
	public:
	bool grab ( ) ;
	bool drop ( ) ;
	
	/* Event Callbacks */
	public:
	virtual void moved ( const MotionEvent & motionEvent ) { }
	virtual void mainButtonPressed ( const ButtonPressEvent & buttonPressEvent ) { }
	virtual void mainButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) { }
	virtual void otherButtonPressed ( const ButtonPressEvent & buttonPressEvent ) { }
	virtual void otherButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) { }
	virtual void grabbed ( const ToolGrabEvent & toolGrabEvent ) { }
	virtual void dropped ( const ToolDropEvent & toolDropEvent ) { }

	/* Elements */
	private:
	ToolBox* mToolBox ;
	ToolList::iterator mToolBoxPosition ;
	Icon* mIcon ;
} ;

} // namespace ToolBox

#endif
