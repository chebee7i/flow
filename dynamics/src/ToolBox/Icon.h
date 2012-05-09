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

#ifndef TOOLBOX_ICON_H
#define TOOLBOX_ICON_H

class GLContextData ;
namespace ToolBox {
	class Tool ;
}

namespace ToolBox {

class Icon
{
	friend class Tool ;

	public:
	virtual ~Icon ( ) { }

	public:
	virtual void display ( GLContextData& contextData ) const = 0 ;

	public:
	Tool* owner ( ) const { return mOwner ; }

	private:
	Tool* mOwner ;
} ;

} // namespace ToolBox

#endif
