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

#ifndef TOOLBOX_EXTENTIONS_TOOLROTATOR_H
#define TOOLBOX_EXTENTIONS_TOOLROTATOR_H

#include <ToolBox/Extension.h>

#include <map>
#include <Geometry/Plane.h>
#include <ToolBox/ToolBox.h> // Only for ButtonId

namespace ToolBox {
namespace Extensions {

class ToolRotator : public Extension
{
	/* (De)Constructors */
	public:
	ToolRotator ( ToolBox* toolBox ) ;
	~ToolRotator ( ) ;

	/* Accessors */
	public:
	const std::string & buttonAlias ( ) const ;
	float iconScale ( ) const ;
//	float iconSpacing ( ) const ;
	float switchDelay ( ) const ;
	float closeDelay ( ) const ;
	float positionOfCurrent ( ) const ;
	bool alwaysShowCurrent ( ) const ;
	bool detachable ( ) const ;

	/* Mutators */
	public:
	ToolRotator & buttonAlias ( const std::string & alias ) ;
	ToolRotator & iconScale ( float iconScale ) ;
//	ToolRotator & iconSpacing ( float iconSpacing ) ;
	ToolRotator & switchDelay ( float switchDelay ) ;
	ToolRotator & closeDelay ( float closeDelay ) ;
	ToolRotator & positionOfCurrent ( float positionOfCurrent ) ;
	ToolRotator & alwaysShowCurrent ( bool alwaysShowCurrent ) ;
	ToolRotator & detachable ( bool detachable ) ;

	/* Event Callbacks */
	public:
	void moved ( const MotionEvent & motionEvent ) ;
	void buttonPressed ( const ButtonPressEvent & buttonPressEvent ) ;
	void buttonReleased ( const ButtonReleaseEvent & buttonReleaseEvent ) ;
	void grabbed ( const ToolGrabEvent & toolGrabEvent ) ;
	void dropped ( const ToolDropEvent & toolDropEvent ) ;
	void added ( const ToolAddEvent & toolAddEvent ) ;
	void removed ( const ToolRemoveEvent & toolRemoveEvent ) ;

	/* Visuals */
	public:
	void frame ( ) ;
	void display ( GLContextData& contextData ) const ;

	/* Utilities */
	private:
	Tool* currentTool ( ) const ;
	void currentToolListPosition ( const ToolList::const_iterator& toolListPosition ) ;
	void update ( ) ;
	void moveToNextTool ( ) ;
	static float minimumDegreeDistance ( float a, float b ) ;
	static Vrui::Scalar * lerp ( Vrui::Scalar * result, Vrui::Scalar t,
                                 const Vrui::Scalar * from, const Vrui::Scalar * to ) ;

	/* Embedded types */
	private:
	enum OpenState { CLOSED, OPENING, OPEN, CLOSING } ;
	enum DetachedState { ATTACHED, DETACHING, DETACHED, ATTACHING } ;
	typedef std::map < Tool*, float > ToolOffsetMap ;

	/* Elements */
	private:
	// User-defined
	std::string mButtonAlias ;
	float mIconScale ;
	float mSwitchDelay ;
	float mCloseDelay ;
	float mPositionOfCurrent ;
	bool mAlwaysShowCurrent ;
	bool mDetachable ;
	// Derived
	ButtonId mButton ;
	ToolList::const_iterator mCurrentToolListPosition ;
	float mIconSpacing ;
	float mThetaDegree ;
	float mRadius ;
	float mRadiusCurrent ;
	float mRotationCurrent ;
	float mRotationGoal ;
	float mRotationTimeCurrent ;
	float mTimeToClose ;
	bool mKeepOpen ;
	bool mRotating ;
	OpenState mOpenState ;
	Vrui::ONTransform mTransformCurrent ;
	Vrui::ONTransform mTransformGoal ;
	float mTransformTimeCurrent ;
	Vrui::ONTransform mTransformAtActivation ;
	float mDetachmentMagnitudeSqr ;
	float mDetachOffset ;
	DetachedState mDetachedState ;
	ToolOffsetMap mToolOffsets ;
} ;

} // namespace Extensions
} // namespace ToolBox

#endif
