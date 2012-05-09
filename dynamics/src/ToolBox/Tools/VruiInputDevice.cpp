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

#include <ToolBox/Tools/VruiInputDevice.h>

#include <string>
#include <Vrui/Vrui.h>
#include <Vrui/InputDevice.h>
#include <Vrui/InputDeviceManager.h>
#include <ToolBox/ToolBox.h>

namespace ToolBox {
namespace Tools {

/*******************
 * VruiInputDevice
 */

/*
 * (De)Constructors
 */

VruiInputDevice::VruiInputDevice ( ToolBox * toolBox, const std::string & name )
	: Tool ( toolBox ),
	  mVruiInputDevice ( NULL )
{
	// For the time being, just make an input device with only one button.
	mVruiInputDevice = Vrui::getInputDeviceManager ( ) ->
		createInputDevice ( name . c_str ( ),
		                    Vrui::InputDevice::TRACK_POS | Vrui::InputDevice::TRACK_DIR | Vrui::InputDevice::TRACK_ORIENT,
		                    1,       // number of buttons
		                    0,       // number of valuators
		                    true ) ; // act like a physical device
	mVruiInputDevice -> setTransformation ( toolBox -> deviceTransformation ( ) ) ;
	mVruiInputDevice -> setDeviceRayDirection ( Vrui::Vector ( 0.0, 1.0, 0.0 ) ) ;
}

VruiInputDevice::~VruiInputDevice ( )
{
	if ( mVruiInputDevice )
		 Vrui::getInputDeviceManager ( ) -> destroyInputDevice ( mVruiInputDevice ) ;
}

/*
 * Event Callbacks
 */

void VruiInputDevice::moved ( const MotionEvent & motionEvent )
{
	if ( mVruiInputDevice )
		mVruiInputDevice -> setTransformation ( toolBox ( ) -> deviceTransformation ( ) ) ;
}

void VruiInputDevice::mainButtonPressed ( const ButtonPressEvent & buttonPressEvent )
{
	if ( mVruiInputDevice )
		mVruiInputDevice -> setButtonState ( 0, true ) ;
	Vrui::requestUpdate ( ) ;
}

void VruiInputDevice::mainButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent )
{
	if ( mVruiInputDevice )
		mVruiInputDevice -> setButtonState ( 0, false ) ;
	Vrui::requestUpdate ( ) ;
}

void VruiInputDevice::otherButtonPressed ( const ButtonPressEvent & buttonPressEvent )
{ }

void VruiInputDevice::otherButtonReleased ( const ButtonReleaseEvent & buttonReleaseEvent )
{ }

void VruiInputDevice::grabbed ( const ToolGrabEvent & toolGrabEvent )
{ }

void VruiInputDevice::dropped ( const ToolDropEvent & toolDropEvent )
{ }

} // namespace Tools
} // namespace ToolBox
