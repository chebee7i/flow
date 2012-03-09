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

#ifndef TOOLBOX_TOOLBOX_H
#define TOOLBOX_TOOLBOX_H

#include <Vrui/Tool.h>

#include <list>
#include <map>
//#include <Misc/CallbackList.h>
#include <Vrui/Geometry.h>
#include <Geometry/OrthogonalTransformation.h> // For NavTrackerState
#include <Geometry/OrthonormalTransformation.h> // For TrackerState
#include <ToolBox/AliasSet.h>
#include <ToolBox/Icon.h>

/* Forward declarations: */
namespace Vrui {
	class ToolManager ;
}
namespace ToolBox {
	class ToolBox;
	class Extension ;
	class Tool ;
}

namespace ToolBox {

typedef std::list < Extension* > ExtensionList ;
typedef std::list < Tool* > ToolList ;
typedef std::pair < int, int > ButtonId ;

class ToolBoxFactory : public Vrui::ToolFactory
{
	friend class ToolBox ;
	friend class Tool ;

	/* Singleton */
	public:
	static ToolBoxFactory * instance ( ) ;

	/* (De)Constructors: */
	private:
	ToolBoxFactory ( ) ;
	ToolBoxFactory ( Vrui::ToolManager & toolManager ) ;
	~ToolBoxFactory ( ) ;

	/* Methods from ToolFactory */
	public:
        const char* getName(void) const ;
        const char* getButtonFunction(int buttonSlotIndex) const ;
	Vrui::Tool* createTool ( const Vrui::ToolInputAssignment& inputAssignment ) const ;
	void destroyTool ( Vrui::Tool* tool ) const ;

	/* Accessors */
	public:
	ButtonId buttonForAlias ( const std::string & alias ) const ;
	const AliasSet& aliasesForButton ( const ButtonId & button ) const ;

	/* Utilities */
	private:
	void initialize ( Vrui::ToolManager & toolManager ) ;

	/* Elements */
	private:
	typedef std::vector < AliasSet > AliasSetVector ;
	typedef std::vector < AliasSetVector > ButtonToAliasesVector ;
        typedef std::map < int, std::string > IntToStrMap ;
	typedef std::map < std::string, ButtonId > AliasToButtonMap ;
	typedef std::list < ToolBox* > ToolBoxList ;

	private:
	ButtonToAliasesVector mButtonToAliases ;
        IntToStrMap mButtonSlotIndexToDescription;
	AliasToButtonMap mAliasToButton ;
	ButtonId mainButton ;
	bool mProjectToScreenDefault ;
	ToolBoxList mToolBoxes ;

	private:
	static ToolBoxFactory * mInstance ;
	
	// Used by the dlopen functions
	public:
	static ToolBoxFactory * createIfNeeded ( Vrui::ToolManager & toolManager ) ;
} ;

class ToolBox : public Vrui::Tool
{
	typedef ::ToolBox::Tool Tool ;

	friend class ToolBoxFactory ;
	friend class Extension ;
	friend class ::ToolBox::Tool ;
	
	/* (De)Constructors: */
	private:
	ToolBox ( const Vrui::ToolFactory* factory,
	          const Vrui::ToolInputAssignment& inputAssignment,
	          bool projectToScreen ) ;
	~ToolBox ( ) ;
	
	/* Methods from Vrui::Tool */
	public:
	const Vrui::ToolFactory* getFactory ( ) const ;
	void buttonCallback ( int buttonIndex, Vrui::InputDevice::ButtonCallbackData* cbData ) ;
	void frame ( ) ;
	void display ( GLContextData& contextData ) const ;

	/* Accessors: */
	public:
	const ExtensionList& extensions ( ) const ;
	const ToolList& tools ( ) const ;
	Tool* currentTool ( ) const ;
	ToolList::const_iterator currentToolListPosition ( ) const ;
	const Vrui::TrackerState& deviceTransformation ( ) const ;
	Vrui::Vector deviceDirection ( ) const ;
	Vrui::Ray deviceRay ( ) const ;
	const Vrui::NavTrackerState& deviceTransformationInModel ( ) const ;
	ButtonId buttonForAlias ( const std::string & alias ) const ;
	const AliasSet& aliasesForButton ( const ButtonId & button ) const ;
	ButtonId mainButton ( ) const ;
	bool projectToScreen ( ) const ;

	/* Mutators */
	public:
	void projectToScreen ( bool projectToScreen ) ;

	/* Actions: */
	private:
	void add ( Extension* extension ) ;
	void remove ( Extension* extension ) ;
	void add ( Tool* tool ) ;
	void remove ( Tool* tool ) ;
	bool makeCurrent ( Tool* tool ) ;
	void updateDeviceTransformations ( ) ;
	template < class EventType >
	void tellExtensions ( void ( Extension::*callback ) ( const EventType & ), const EventType & event ) ;

	/* Elements: */
	private:
	ExtensionList mExtensions ;
	ToolList mTools ;
	ToolList::iterator mCurrentTool ;
	Vrui::TrackerState mDeviceTransformation ;
	Vrui::NavTrackerState mDeviceTransformationInModel ;
	bool mProjectToScreen ;
	Icon* mDefaultIcon ;
} ;

} // namespace ToolBox

#endif
