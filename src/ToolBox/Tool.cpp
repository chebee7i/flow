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

#include <ToolBox/Tool.h>

#include <iostream> // DEBUG

#include <ToolBox/ToolBox.h>
#include <ToolBox/Icon.h>

namespace ToolBox {

/*
 * (De)Constructors
 */

Tool::Tool ( ToolBox* toolBox )
	: mToolBox ( toolBox ), mIcon ( NULL )
{
	mToolBox -> add ( this ) ;
}

Tool::~Tool ( )
{
	mToolBox -> remove ( this ) ;
	if ( mIcon )
		delete mIcon ;
}

/*
 * Accessors
 */

ToolBox* Tool::toolBox ( ) const
{
	return mToolBox ;
}

bool Tool::current ( ) const
{
	return mToolBox -> currentTool ( ) == this ;
}

Icon* Tool::icon ( ) const
{
	if ( mIcon )
		return mIcon ;
	else
		return toolBox ( ) -> mDefaultIcon ;
}

/*
 * Mutators
 */

void Tool::icon ( Icon* icon )
{
	if ( mIcon )
		delete mIcon ;
	mIcon = icon ;
	mIcon -> mOwner = this ;
}

/*
 * Actions
 */

bool Tool::grab ( )
{
	return mToolBox -> makeCurrent ( this ) ;
}

bool Tool::drop ( )
{
	return mToolBox -> makeCurrent ( NULL ) ;
}

} // namespace ToolBox
