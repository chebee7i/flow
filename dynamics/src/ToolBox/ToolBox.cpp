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

#include <ToolBox/ToolBox.h>

#include <iostream> // DEBUG

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <Misc/StandardValueCoders.h>
#include <Misc/CompoundValueCoders.h>
#include <Misc/ConfigurationFile.h>
#include <Geometry/Ray.h> // Needed for projection to screen
#include <Vrui/Vrui.h>
#include <Vrui/ToolManager.h>
#include <ToolBox/Events.h>
#include <ToolBox/Extension.h>
#include <ToolBox/Tool.h>

namespace ToolBox {

/*************************************
 * Declaration of class DefaultIcon:
 */

class DefaultIcon : public Icon
{
	void display ( GLContextData& contextData ) const ;
} ;

/**********************************
 * Methods of class ToolBoxFactory:
 */

extern "C" void destroyToolBoxFactory ( Vrui::ToolFactory * ) ;

ToolBoxFactory * ToolBoxFactory::mInstance = NULL ;

/*
 * Singleton
 */

ToolBoxFactory * ToolBoxFactory::instance ( )
{
	if ( ! mInstance )
		mInstance = new ToolBoxFactory ( ) ;
	return mInstance ;
}

/*
 * (De)Constructors
 */

ToolBoxFactory::ToolBoxFactory ( )
	: ToolFactory ( "ToolBox", * Vrui::getToolManager ( ) )
{
	initialize ( * Vrui::getToolManager ( ) ) ;
	Vrui::getToolManager ( ) ->	addClass ( this, destroyToolBoxFactory ) ;
}

ToolBoxFactory::ToolBoxFactory ( Vrui::ToolManager & toolManager )
	: ToolFactory ( "ToolBox", toolManager )
{
	initialize ( toolManager ) ;
}

ToolBoxFactory::~ToolBoxFactory ( )
{
	mInstance = NULL ;
}

/*
 * Methods from ToolFactory
 */

const char* ToolBoxFactory::getName(void) const
{
    return "ToolBox";
}


Vrui::Tool* ToolBoxFactory::createTool ( const Vrui::ToolInputAssignment& inputAssignment ) const
{
	return new ToolBox ( this, inputAssignment, mProjectToScreenDefault ) ;
}

void ToolBoxFactory::destroyTool ( Vrui::Tool* tool ) const
{
	delete tool ;
}

const char* ToolBoxFactory::getButtonFunction(int buttonSlotIndex) const
{
	// cannot use [] operator since that is not declared const
	std::string desc = mButtonSlotIndexToDescription.find(buttonSlotIndex)->second ;
	return desc.c_str();
}

/*
 * Accessors
 */

ButtonId ToolBoxFactory::buttonForAlias ( const std::string & alias ) const
{
	AliasToButtonMap::const_iterator pos ( mAliasToButton . find ( alias ) ) ;
	if ( pos == mAliasToButton . end ( ) )
		throw std::runtime_error ( "ToolBox::ToolBoxFactory::buttonForAlias : alias '" + alias + "' not found" ) ;
	return pos -> second ;
}

const AliasSet& ToolBoxFactory::aliasesForButton ( const ButtonId & button ) const
{
	if ( (int) mButtonToAliases . size ( ) <= button . first )
		throw std::runtime_error ( "ToolBox::ToolBoxFactory::aliasesForButton : device not found" ) ;
	if ( (int) mButtonToAliases [ button . first ] . size ( ) <= button . second )
		throw std::runtime_error ( "ToolBox::ToolBoxFactory::aliasesForButton : button not found" ) ;
	return mButtonToAliases [ button . first ] [ button . second ] ;
}

/*
 * Utilities
 */

void ToolBoxFactory::initialize ( Vrui::ToolManager & toolManager )
{

	// Load class settings:
	Misc::ConfigurationFileSection configFileSection ( toolManager . getToolClassSection ( getClassName ( ) ) ) ;
	mProjectToScreenDefault = configFileSection . retrieveValue < bool > ( "./projectToScreen", false ) ;
	int numberOfButtons ( configFileSection . retrieveValue < int > ( "./numberOfButtons", 3 ) ) ;
	
	// blah, ugly hack
	std::vector< std::list<std::string> > defaultNames;
	std::list<std::string> tmp0, tmp1, tmp2;
	tmp0.push_back("main");
	defaultNames.push_back( tmp0 );
	tmp1.push_back("alt");
	defaultNames.push_back( tmp1 );
	tmp2.push_back("switch");
	defaultNames.push_back( tmp2 );
	
	typedef std::list < std::string > StringList ;
	mButtonToAliases . push_back ( AliasSetVector ( ) ) ;
	for ( int buttonIndex ( 0 ); buttonIndex < numberOfButtons; ++buttonIndex )
	{
		mButtonToAliases [ 0 ] . push_back ( AliasSet ( ) ) ;

		std::stringstream buttonIndexTag;
		buttonIndexTag << "./buttonIndex" << buttonIndex;
		StringList aliases (
			configFileSection . retrieveValue < StringList > ( buttonIndexTag.str().c_str(), defaultNames[buttonIndex] ) ) ;

		std::stringstream buttonDescriptionTag;
		buttonDescriptionTag << "./buttonDescription" << buttonIndex;
		std::string desc = configFileSection . retrieveValue < std::string > ( buttonDescriptionTag.str().c_str(), defaultNames[buttonIndex].front() ) ;
		mButtonSlotIndexToDescription.insert( std::make_pair( buttonIndex, desc ) ) ;

		for ( StringList::iterator alias ( aliases . begin ( ) ) ; alias != aliases . end ( ) ; ++alias ) {
			mButtonToAliases [ 0 ] [ buttonIndex ] . insert ( *alias ) ;
			mAliasToButton . insert ( std::make_pair ( *alias, ButtonId ( 0, buttonIndex ) ) ) ;
		}
	}
	mainButton = buttonForAlias ( "main" ) ;
	

	// Initialize tool layout:
	layout . setNumButtons ( numberOfButtons ) ;

	// Set tool class' factory pointer:
	mInstance = this ;
}

/*
 * Functions for dlsym
 */

ToolBoxFactory * ToolBoxFactory::createIfNeeded ( Vrui::ToolManager & toolManager )
{
	if ( ! mInstance )
    {
		if ( ! Vrui::getToolManager ( ) )
        {
			mInstance = new ToolBoxFactory ( toolManager ) ;
        }
		else
        {
			mInstance = new ToolBoxFactory ( ) ;
        }
    }
	return mInstance ;
}

extern "C" void resolveToolBoxDependencies ( Plugins::FactoryManager < Vrui::ToolFactory > & manager )
{
}

extern "C" Vrui::ToolFactory* createToolBoxFactory ( Plugins::FactoryManager < Vrui::ToolFactory > & manager )
{
	// Get pointer to tool manager:
	Vrui::ToolManager* toolManager = static_cast < Vrui::ToolManager* > ( & manager ) ;

	// Create factory object and insert it into class hierarchy:
	ToolBoxFactory* toolBoxFactory = ToolBoxFactory::createIfNeeded ( * toolManager ) ;

	// Return factory object:
	return toolBoxFactory;
}

extern "C" void destroyToolBoxFactory ( Vrui::ToolFactory * factory )
{
	delete factory;
}

/***************************
 * Methods of class ToolBox:
 */

/*
 * (De)Constructors
 */

ToolBox::ToolBox ( const Vrui::ToolFactory* factory,
                   const Vrui::ToolInputAssignment& inputAssignment,
                   bool projectToScreen )
	: Vrui::Tool ( factory, inputAssignment ),
	  mTools ( ), mCurrentTool ( mTools . begin ( ) ),
	  mProjectToScreen ( projectToScreen ),
	  mDefaultIcon ( new DefaultIcon )
{ }

ToolBox::~ToolBox ( )
{
	while ( ! mExtensions . empty ( ) ) {
		delete mExtensions . front ( ) ;
	}
	while ( ! mTools . empty ( ) ) {
		delete mTools . front ( ) ;
	}
	if ( mDefaultIcon )
		delete mDefaultIcon ;
}

/*
 * Methods from Vrui::Tool
 */

const Vrui::ToolFactory* ToolBox::getFactory ( ) const
{
	return ToolBoxFactory::instance ( ) ;
}

void ToolBox::buttonCallback ( int buttonIndex, Vrui::InputDevice::ButtonCallbackData* cbData )
{
	updateDeviceTransformations ( ) ;
	int deviceIndex = 0;
	ButtonId button ( deviceIndex, buttonIndex ) ;
	if ( cbData -> newButtonState )
	{
		ButtonPressEvent buttonPressEvent ( this, currentTool ( ), button ) ;
		tellExtensions ( & Extension::buttonPressed, buttonPressEvent ) ;
		if ( currentTool ( ) ) {
			// In case an extension modified ToolBox state:
			buttonPressEvent . tool = currentTool ( ) ;

			if ( button == mainButton ( ) )
				currentTool ( ) -> mainButtonPressed ( buttonPressEvent ) ;
			else
				currentTool ( ) -> otherButtonPressed ( buttonPressEvent ) ;
		}
	}
	else
	{
		ButtonReleaseEvent buttonReleaseEvent ( this, currentTool ( ), button ) ;
		tellExtensions ( & Extension::buttonReleased, buttonReleaseEvent ) ;
		if ( currentTool ( ) ) {
			// In case an extension modified ToolBox state:
			buttonReleaseEvent . tool = currentTool ( ) ;

			if ( button == mainButton ( ) )
				currentTool ( ) -> mainButtonReleased ( buttonReleaseEvent ) ;
			else
				currentTool ( ) -> otherButtonReleased ( buttonReleaseEvent ) ;
		}
	}
}

void ToolBox::frame ( )
{
	updateDeviceTransformations ( ) ;

	MotionEvent motionEvent ( this, currentTool ( ) ) ;

	tellExtensions ( & Extension::moved, motionEvent ) ;

	// In case an extension modified ToolBox state:
	motionEvent . tool = currentTool ( ) ;

	if ( currentTool ( ) )
		currentTool ( ) -> moved ( motionEvent ) ;

	for ( ExtensionList::iterator pos ( mExtensions . begin ( ) ) ;
	      pos != mExtensions . end ( ) ; ++pos )
	{
		(*pos) -> frame ( ) ;
	}
}

void ToolBox::display ( GLContextData& contextData ) const
{
	for ( ExtensionList::const_iterator pos ( mExtensions . begin ( ) ) ;
	      pos != mExtensions . end ( ) ; ++pos )
	{
		(*pos) -> display ( contextData ) ;
	}
}

/*
 * Accessors
 */

const ExtensionList& ToolBox::extensions ( ) const
{
	return mExtensions ;
}

const ToolList& ToolBox::tools ( ) const
{
	return mTools ;
}

Tool* ToolBox::currentTool ( ) const
{
	return mCurrentTool == mTools . end ( ) ? NULL : *mCurrentTool ;
}

ToolList::const_iterator ToolBox::currentToolListPosition ( ) const
{
	return mCurrentTool ;
}

const Vrui::TrackerState& ToolBox::deviceTransformation ( ) const
{
	return mDeviceTransformation ;
}

Vrui::Vector ToolBox::deviceDirection ( ) const
{
	return input . getSlotDevice ( 0 ) -> getRayDirection ( ) ;
}

Vrui::Ray ToolBox::deviceRay ( ) const
{
	return Vrui::Ray ( deviceTransformation ( ) . getOrigin ( ),
	                   input . getSlotDevice ( 0 ) -> getRayDirection ( ) ) ;
}

const Vrui::NavTrackerState& ToolBox::deviceTransformationInModel ( ) const
{
	return mDeviceTransformationInModel ;
}

ButtonId ToolBox::buttonForAlias ( const std::string & alias ) const
{
	return ToolBoxFactory::instance ( ) -> buttonForAlias ( alias ) ;
}

const AliasSet& ToolBox::aliasesForButton ( const ButtonId & button ) const
{
	return ToolBoxFactory::instance ( ) -> aliasesForButton ( button ) ;
}

ButtonId ToolBox::mainButton ( ) const
{
	return ToolBoxFactory::instance ( ) -> mainButton ;
}

bool ToolBox::projectToScreen ( ) const
{
	return mProjectToScreen ;
}

/*
 * Mutators
 */

void ToolBox::projectToScreen ( bool projectToScreen )
{
	mProjectToScreen = projectToScreen ;
}

/*
 * Actions
 */

void ToolBox::add ( Extension* extension )
{
	mExtensions . push_back ( extension ) ;
	extension -> mToolBoxPosition = -- mExtensions . end ( ) ;
}

void ToolBox::remove ( Extension* extension )
{
	mExtensions . erase ( extension -> mToolBoxPosition ) ;
	extension -> mToolBoxPosition = mExtensions . end ( ) ;
}

void ToolBox::add ( Tool* tool )
{
	mTools . push_back ( tool ) ;
	tool -> mToolBoxPosition = -- mTools . end ( ) ;
	tellExtensions ( & Extension::added, ToolAddEvent ( this, tool ) ) ;
}

void ToolBox::remove ( Tool* tool )
{
	if ( *mCurrentTool == tool )
		makeCurrent ( NULL ) ;
	mTools . erase ( tool -> mToolBoxPosition ) ;
	tool -> mToolBoxPosition = mTools . end ( ) ;
	tellExtensions ( & Extension::removed, ToolRemoveEvent ( this, tool ) ) ;
}

bool ToolBox::makeCurrent ( Tool* tool )
{
	// Ignore call to grab current tool:
	if ( currentTool ( ) == tool )
		return false ;

	ToolDropEvent toolDropEvent ( this, currentTool ( ) ) ;
	if ( currentTool ( ) ) {
		mCurrentTool = mTools . end ( ) ;
		tellExtensions ( & Extension::dropped, toolDropEvent ) ;
		toolDropEvent . tool -> dropped ( toolDropEvent ) ;
	}
	if ( tool ) {
		mCurrentTool = tool -> mToolBoxPosition ;
		ToolGrabEvent toolGrabEvent ( this, tool ) ;
		tellExtensions ( & Extension::grabbed, toolGrabEvent ) ;
		tool -> grabbed ( toolGrabEvent ) ;
	}
	return true ;
}

void ToolBox::updateDeviceTransformations ( )
{
	if ( mProjectToScreen )
	{
		// Get pointer to input device:
		Vrui::InputDevice* device ( input . getSlotDevice ( 0 ) ) ;

		// Calculate ray equation:
		Vrui::Ray deviceRay ( device -> getPosition ( ), device -> getRayDirection ( ) ) ;

		// Find the closest intersection with any screen:
		std::pair < Vrui::VRScreen*, Vrui::Scalar > screenIntersection ( Vrui::findScreen ( deviceRay ) ) ;

		// Set the current transformation to the input device:
		mDeviceTransformation = input . getSlotDevice ( 0 ) -> getTransformation ( ) ;
		if ( screenIntersection . first != 0 )
		{
			// Translate the input device's transformation to have its origin on the screen:
			mDeviceTransformation . leftMultiply (
				Vrui::TrackerState::translate ( deviceRay . getDirection ( ) * screenIntersection . second ) ) ;
		}

		// Convert current transformation to navigation coordinates:
		mDeviceTransformationInModel = mDeviceTransformation ;
		mDeviceTransformationInModel . leftMultiply ( Vrui::getInverseNavigationTransformation ( ) ) ;
	}
	else
	{
		mDeviceTransformation = input . getSlotDevice ( 0 ) -> getTransformation ( ) ;
		mDeviceTransformationInModel = Vrui::getDeviceTransformation ( input . getSlotDevice ( 0 ) ) ;
	}
}

template < class EventType >
void ToolBox::tellExtensions ( void ( Extension::*callback ) ( const EventType & ), const EventType & event )
{
	for ( ExtensionList::iterator pos ( mExtensions . begin ( ) ) ; pos != mExtensions . end ( ) ; ++pos )
		( (*pos) ->* callback ) ( event ) ;
}

} // namespace ToolBox

/***************************
 * Methods of class ToolBox:
 */

#include <GL/GLContextData.h>
#include <GL/GLMaterial.h>
#include <GL/GLModels.h>
#include <GL/GLGeometryWrappers.h>

namespace ToolBox {

void DefaultIcon::display ( GLContextData& contextData ) const
{
	glMaterial ( GLMaterialEnums::FRONT_AND_BACK, GLMaterial ( Vrui::getUiBgColor ( ) ) ) ;
	glPushMatrix ( ) ;
	glTranslate ( 0.0f, 0.0f, 0.0f ) ;
	glRotate ( Vrui::Scalar ( 50.0 ), Vrui::Vector ( -1.0, -1.0, 1.0 ) ) ;
	glDrawCone ( 0.75, 1.5, 10 ) ;
	glPopMatrix ( ) ;
}

} // namespace ToolBox
