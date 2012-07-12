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

#ifndef TOOLBOX_EXTENSION_H
#define TOOLBOX_EXTENSION_H

#include <list>

class GLContextData ;

namespace ToolBox {
	class Extension ;
	class ToolBox ;
	class MotionEvent ;
	class ButtonPressEvent ;
	class ButtonReleaseEvent ;
	class ToolGrabEvent ;
	class ToolDropEvent ;
	class ToolAddEvent ;
	class ToolRemoveEvent ;
}

namespace ToolBox {

typedef std::list < Extension* > ExtensionList ;

class Extension
{
	friend class ToolBox ;

	/* (De)Constructors */
	public:
	Extension ( ToolBox* toolBox ) ;
	virtual ~Extension ( ) ;

	/* Accessors */
	public:
	ToolBox* toolBox ( ) const ;

	/* Event Callbacks */
	public:
	virtual void moved ( const MotionEvent & motionEvent ) { }
	virtual void buttonPressed ( const ButtonPressEvent & buttonPressEvent ) { }
	virtual void buttonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) { }
	virtual void grabbed ( const ToolGrabEvent & toolGrabEvent ) { }
	virtual void dropped ( const ToolDropEvent & toolDropEvent ) { }
	virtual void added ( const ToolAddEvent & toolAddEvent ) { }
	virtual void removed ( const ToolRemoveEvent & toolRemoveEvent ) { }

	/* Visuals */
	public:
	virtual void frame ( ) { }
	virtual void display ( GLContextData& contextData ) const { }

	/* Elements */
	private:
	ToolBox* mToolBox ;
	ExtensionList::iterator mToolBoxPosition ;
} ;

} // namespace ToolBox

#endif
