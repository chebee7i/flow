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

#ifndef TOOLBOX_ALIASSET_H
#define TOOLBOX_ALIASSET_H

#include <string>
#include <set>

namespace ToolBox {

struct CaseInsensitiveStringLess {
	bool operator() ( const std::string& s1, const std::string& s2 ) const
	{
		std::string::const_iterator pos1 ( s1 . begin ( ) ) ;
		std::string::const_iterator pos2 ( s2 . begin ( ) ) ;
		for ( ; pos1 != s1 . end ( ) && pos2 != s2 . end ( ) ; ++pos1, ++pos2 )
			if ( std::tolower ( *pos1 ) != std::tolower ( *pos2 ) )
				return std::tolower ( *pos1 ) < std::tolower ( *pos2 ) ;
		return s1 . size ( ) < s2 . size ( ) ;
	}
} ;

struct CaseInsensitiveStringEqual {
	bool operator() ( const std::string& s1, const std::string& s2 ) const
	{
		if ( s1 . size ( ) != s2 . size ( ) )
			return false ;
		std::string::const_iterator pos1 ( s1 . begin ( ) ) ;
		std::string::const_iterator pos2 ( s2 . begin ( ) ) ;
		for ( ; pos1 != s1 . end ( ) && pos2 != s2 . end ( ) ; ++pos1, ++pos2 )
			if ( std::tolower ( *pos1 ) != std::tolower ( *pos2 ) )
				return false ;
		return true ;
	}
} ;

typedef std::set < std::string, CaseInsensitiveStringLess > AliasSet ;

} // namespace ToolBox

#endif
