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

#include <ToolBox/Extension.h>

#include <ToolBox/ToolBox.h>

namespace ToolBox {

Extension::Extension ( ToolBox* toolBox )
	: mToolBox ( toolBox )
{
	mToolBox -> add ( this ) ;
}

Extension::~Extension ( )
{
	mToolBox -> remove ( this ) ;
}

ToolBox* Extension::toolBox ( ) const
{
	return mToolBox ;
}

} // namespace ToolBox
