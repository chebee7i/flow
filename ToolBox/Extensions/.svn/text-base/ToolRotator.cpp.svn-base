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

#include <ToolBox/Extensions/ToolRotator.h>

#include <iostream> // DEBUG
#include <GL/GLModels.h> // DEBUG
#include <GL/GLGeometryWrappers.h> // DEBUG

#include <Math/Math.h>
#include <Math/Constants.h>
#include <GL/GLContextData.h> // To show icons
#include <GL/GLGeometryWrappers.h> // To show icons
#include <GL/GLTransformationWrappers.h> // To show icons
#include <Vrui/Vrui.h>
#include <ToolBox/Events.h>
#include <ToolBox/Tool.h>
#include <ToolBox/Icon.h> // To show icons

namespace ToolBox {
namespace Extensions {

/*
 * (De)Constructors
 */

ToolRotator::ToolRotator ( ToolBox* toolBox )
	: Extension ( toolBox ),
	  mButtonAlias ( "switch" ),
	  mIconScale ( 2.0 * Vrui::getUiSize ( ) ),
//	  mIconSpacing ( 0.5 ),
	  mSwitchDelay ( 0.3 ),
	  mCloseDelay ( 1.0 ),
	  mPositionOfCurrent ( 0.0 ),
	  mAlwaysShowCurrent ( true ),
	  mDetachable ( true ),
	  mButton ( toolBox -> buttonForAlias ( mButtonAlias ) ),
	  mCurrentToolListPosition ( toolBox -> tools ( ) . end ( ) ),
	  mRadiusCurrent ( 0.0 ),
	  mRotationCurrent ( 0.0 ),
	  mTimeToClose ( 0.0 ),
	  mKeepOpen ( false ),
	  mRotating ( false ),
	  mOpenState ( CLOSED ),
	  mDetachedState ( ATTACHED )
{
	currentToolListPosition ( toolBox -> currentToolListPosition ( ) ) ;
	update ( ) ;
}

ToolRotator::~ToolRotator ( )
{ }

/*
 * Accessors
 */

const std::string & ToolRotator::buttonAlias ( ) const
{
	return mButtonAlias ;
}

float ToolRotator::iconScale ( ) const
{
	return mIconScale / Vrui::getUiSize ( ) ;
}

//float ToolRotator::iconSpacing ( ) const
//{
//	return mIconSpacing ;
//}

float ToolRotator::switchDelay ( ) const
{
	return mSwitchDelay ;
}

float ToolRotator::closeDelay ( ) const
{
	return mCloseDelay ;
}

float ToolRotator::positionOfCurrent ( ) const
{
	return mPositionOfCurrent ;
}

bool ToolRotator::alwaysShowCurrent ( ) const
{
	return mAlwaysShowCurrent ;
}

bool ToolRotator::detachable ( ) const
{
	return mDetachable ;
}

/*
 * Mutators
 */

ToolRotator & ToolRotator::buttonAlias ( const std::string & alias )
{
	mButtonAlias = alias ;
	mButton = toolBox ( ) -> buttonForAlias ( alias ) ;
	return * this ;
}

ToolRotator & ToolRotator::iconScale ( float iconScale )
{
	mIconScale = iconScale * Vrui::getUiSize ( ) ;
	update ( ) ;
	return * this ;
}

//ToolRotator & ToolRotator::iconSpacing ( float iconSpacing )
//{
//	mIconSpacing = iconSpacing ;
//	update ( ) ;
//	return * this ;
//}

ToolRotator & ToolRotator::switchDelay ( float switchDelay )
{
	mSwitchDelay = switchDelay ;
	return * this ;
}

ToolRotator & ToolRotator::closeDelay ( float closeDelay )
{
	mCloseDelay = closeDelay ;
	return * this ;
}

ToolRotator & ToolRotator::positionOfCurrent ( float positionOfCurrent )
{
	mPositionOfCurrent = positionOfCurrent ;
	return * this ;
}

ToolRotator & ToolRotator::alwaysShowCurrent ( bool alwaysShowCurrent )
{
	mAlwaysShowCurrent = alwaysShowCurrent ;
	return * this ;
}

ToolRotator & ToolRotator::detachable ( bool detachable )
{
	mDetachable = detachable ;
	return * this ;
}

/*
 * Event Callbacks
 */

void ToolRotator::moved ( const MotionEvent & motionEvent )
{
	// If the wheel is away, we know we can get out immediately.
	if ( mOpenState == CLOSED )
		return ;

	// The wheel is detached, so calculate where the user
	// is pointing and make the corresponding tool current.
	if ( mOpenState == OPEN && mDetachedState == DETACHED )
	{
		Vrui::Ray deviceRay ( toolBox ( ) -> deviceRay ( ) . inverseTransform (
			mTransformCurrent *
			Vrui::ONTransform ( Vrui::Vector::zero,
			                    Vrui::Rotation ( Vrui::Vector ( 0, 1, 0 ), Math::rad(mPositionOfCurrent) ) ) *
			Vrui::ONTransform ( Vrui::Vector ( 0, 0, - mRadiusCurrent ), Vrui::Rotation::identity )
			) ) ;
		Vrui::Plane plane ( Vrui::Vector ( 0, 1, 0 ), 0 ) ;
		Vrui::Plane::HitResult result ( plane . intersectRay ( deviceRay ) ) ;

		if ( result . isValid ( ) ) {
			Vrui::Point hit = deviceRay ( result . getParameter ( ) ) ;
			float hitDegree ( Math::deg ( atan2 ( hit [ 0 ], - hit [ 2 ] ) ) + 180.0 ) ;
			hitDegree = Math::mod ( hitDegree + mRotationCurrent, 360.0f ) ;
			int toolIndex ( int ( ( hitDegree / mThetaDegree ) + 0.5 ) %
			                ( toolBox ( ) -> tools ( ) . size ( ) ) ) ;
			ToolList::const_iterator pos ( toolBox ( ) -> tools ( ) . begin ( ) ) ;
			for ( int i ( 0 ) ; i < toolIndex ; ++i, ++pos ) ; // get tool at index
			currentToolListPosition ( pos ) ;
		}
	}

	// The wheel is attaching or attaching, so make sure its position
	// tracks with the input device's position.
	if ( mDetachedState == ATTACHED || mDetachedState == ATTACHING )
		mTransformGoal = toolBox ( ) -> deviceTransformation ( ) ;

	// Check to see if the wheel should detach.
	if ( mDetachable && mKeepOpen && mDetachedState == ATTACHED ) {
		if ( ( mTransformAtActivation . getOrigin ( ) -
		       toolBox ( ) -> deviceTransformation ( ) . getOrigin ( ) ) . sqr ( ) >
		         mDetachmentMagnitudeSqr )
		{
			Vrui::Vector offset ( Geometry::normalize ( toolBox ( ) -> deviceDirection ( ) ) ) ;
			offset *= mDetachOffset ;
			mTransformGoal . getTranslation ( ) += offset ;
			mTransformTimeCurrent = mSwitchDelay ;
			mDetachedState = DETACHING ;
		}
	}
}

void ToolRotator::buttonPressed ( const ButtonPressEvent & buttonPressEvent )
{
	if ( buttonPressEvent . button == mButton )
	{
		if ( mDetachedState == ATTACHED )
		{
			mToolOffsets . clear ( ) ;
			mKeepOpen = true ;
			mTransformAtActivation = toolBox ( ) -> deviceTransformation ( ) ;
			if ( mAlwaysShowCurrent || mOpenState == OPEN || mOpenState == OPENING )
				moveToNextTool ( ) ;
			else
				mOpenState = OPENING ;
			if ( ! currentTool ( ) && ! toolBox ( ) -> tools ( ) . empty ( ) )
				currentToolListPosition ( toolBox ( ) -> tools ( ) . begin ( ) ) ;
		}
	}
	else
	{
		if ( currentTool ( ) )
			currentTool ( ) -> grab ( ) ;
		mKeepOpen = false ;
		mTimeToClose = 0.0 ;
	}
}

void ToolRotator::buttonReleased ( const ButtonReleaseEvent & buttonReleaseEvent )
{
	if ( buttonReleaseEvent . button == mButton )
		mKeepOpen = false ;
}

void ToolRotator::grabbed ( const ToolGrabEvent & toolGrabEvent )
{
	// Sync our current tool with the actual current tool:
	currentToolListPosition ( toolBox ( ) -> currentToolListPosition ( ) ) ;
}

void ToolRotator::dropped ( const ToolDropEvent & toolDropEvent )
{
}

void ToolRotator::added ( const ToolAddEvent & toolAddEvent )
{
	// Sync our current tool with the actual current tool:
	currentToolListPosition ( toolBox ( ) -> currentToolListPosition ( ) ) ;
}

void ToolRotator::removed ( const ToolRemoveEvent & toolRemoveEvent )
{
	mToolOffsets . erase ( toolRemoveEvent . tool ) ;
	// Sync our current tool with the actual current tool:
	currentToolListPosition ( toolBox ( ) -> currentToolListPosition ( ) ) ;
}

/*
 * Visuals
 */

void ToolRotator::frame ( )
{
	if ( mOpenState != CLOSED && mDetachedState != DETACHED )
	{
		float distance ( minimumDegreeDistance ( mRotationCurrent, mRotationGoal ) ) ;
		float absDistance ( Math::abs ( distance ) ) ;
		float minimumSpeed ( mThetaDegree / mSwitchDelay ) ;
		if ( absDistance < ( mRotationTimeCurrent * minimumSpeed ) )
			mRotationTimeCurrent = absDistance / minimumSpeed ;
		if ( Vrui::getCurrentFrameTime ( ) >= mRotationTimeCurrent ) {
			mRotationTimeCurrent = 0.0 ;
			mRotationCurrent = mRotationGoal ;
			mRotating = false ;
		} else {
			mRotationCurrent += distance * Vrui::getCurrentFrameTime ( ) / mRotationTimeCurrent ;
			mRotationTimeCurrent -= Vrui::getCurrentFrameTime ( ) ;
			mRotating = true ;
			Vrui::requestUpdate() ;
		}
	}

	switch ( mOpenState )
	{
		case CLOSED :
		{
			break;
		}
		case OPENING :
		{
			mRadiusCurrent += Vrui::getCurrentFrameTime ( ) * mRadius / mSwitchDelay ;
			if ( mRadiusCurrent > mRadius ) {
				mRadiusCurrent = mRadius ;
				mTimeToClose = mCloseDelay ;
				mOpenState = OPEN ;
			}
			Vrui::requestUpdate() ;
			break ;
		}
		case OPEN :
		{
			if ( ! mRotating ) {
				if ( ! mKeepOpen && mDetachedState == ATTACHED ) {
					mTimeToClose -= Vrui::getCurrentFrameTime ( ) ;
					Vrui::requestUpdate() ;
				}
				if ( mTimeToClose <= 0.0 ) {
					mTimeToClose = 0.0 ;
					mOpenState = CLOSING ;
					Vrui::requestUpdate() ;
				}
			}
			break ;
		}
		case CLOSING :
		{
			mRadiusCurrent -= Vrui::getCurrentFrameTime ( ) * mRadius / mSwitchDelay ;
			if ( mRadiusCurrent <= 0.0 ) {
				mRadiusCurrent = 0.0 ;
				mOpenState = CLOSED ;
				if ( currentTool ( ) )
					currentTool ( ) -> grab ( ) ;
			}
			Vrui::requestUpdate() ;
			break ;
		}
	}


	switch ( mDetachedState )
	{
		case ATTACHED :
		{
			mTransformCurrent = mTransformGoal ;
			break ;
		}
		case DETACHING :
		{
			if ( Vrui::getCurrentFrameTime ( ) >= mTransformTimeCurrent ) {
				mTransformTimeCurrent = 0.0 ;
				mTransformCurrent = mTransformGoal ;
				if ( currentTool ( ) )
					mToolOffsets [ currentTool ( ) ] = 0.0 ;
				mDetachedState = DETACHED ;
			} else {
				Vrui::Vector increment ( ( Geometry::invert ( mTransformCurrent ) * mTransformGoal ) . getTranslation ( ) ) ;
				increment *= Vrui::getCurrentFrameTime ( ) / mTransformTimeCurrent ;
				mTransformCurrent . getTranslation ( ) += increment ;
				mTransformTimeCurrent -= Vrui::getCurrentFrameTime ( ) ;
			}
			Vrui::requestUpdate() ;
			break ;
		}
		case DETACHED :
		{
			if ( ! mKeepOpen ) {
				mTransformTimeCurrent = mSwitchDelay ;
				mDetachedState = ATTACHING ;
			}
			for ( ToolOffsetMap::iterator pos ( mToolOffsets . begin ( ) ) ; pos != mToolOffsets . end ( ) ; ++pos )
			{
				if ( pos -> first == currentTool ( ) && pos -> second < mDetachOffset ) {
					pos -> second += mDetachOffset * ( Vrui::getCurrentFrameTime ( ) / mSwitchDelay ) ;
					if ( pos -> second > mDetachOffset )
						pos -> second = mDetachOffset ;
				} else if ( pos -> first != currentTool ( ) && pos -> second > 0.0 ) {
					pos -> second -= mDetachOffset * ( Vrui::getCurrentFrameTime ( ) / mSwitchDelay ) ;
					if ( pos -> second <= 0.0 )
						mToolOffsets . erase ( pos -> first ) ;
				}
			}
			Vrui::requestUpdate() ;
			break ;
		}
		case ATTACHING :
		{
			if ( Vrui::getCurrentFrameTime ( ) >= mTransformTimeCurrent ) {
				mTransformTimeCurrent = 0.0 ;
				mTransformCurrent = mTransformGoal ;
				mDetachedState = ATTACHED ;
			} else {
				// Interpolation amount:
				float step ( Vrui::getCurrentFrameTime ( ) / mTransformTimeCurrent ) ;
				// Interpolate translation:
				Vrui::Vector translationStep (   mTransformGoal . getTranslation ( )
				                               - mTransformCurrent . getTranslation ( ) ) ;
				translationStep *= step ;
				mTransformCurrent . getTranslation ( ) += translationStep ;
				// Interpolate rotation:			
				Vrui::Scalar newRotation [ 4 ] ;
				lerp ( newRotation,
				       step,
				       mTransformCurrent . getRotation ( ) . getQuaternion ( ),
				       mTransformGoal . getRotation ( ) . getQuaternion ( ) ) ;
				mTransformCurrent . getRotation ( ) = Vrui::Rotation::fromQuaternion ( newRotation ) ;
				// Decrement countdown to goal:
				mTransformTimeCurrent -= Vrui::getCurrentFrameTime ( ) ;
			}
			for ( ToolOffsetMap::iterator pos ( mToolOffsets . begin ( ) ) ; pos != mToolOffsets . end ( ) ; ++pos )
			{
				pos -> second -= mDetachOffset * ( Vrui::getCurrentFrameTime ( ) / mSwitchDelay ) ;
				if ( pos -> second <= 0.0 )
					mToolOffsets . erase ( pos -> first ) ;
			}
			Vrui::requestUpdate() ;
			break ;
		}
	}

}

void ToolRotator::display ( GLContextData& contextData ) const
{
	const ToolList & toolList ( toolBox ( ) -> tools ( ) ) ;

	if ( mRotationCurrent == mRotationGoal && mOpenState == CLOSED )
	{
		if ( mAlwaysShowCurrent && currentTool ( ) )
		{
			glPushMatrix ( ) ;
			glMultMatrix ( toolBox ( ) -> deviceTransformation ( ) ) ;
			glScale ( mIconScale ) ;
			currentTool ( ) -> icon ( ) -> display ( contextData ) ;
			glPopMatrix ( ) ;
		}
	}
	else
	{
		glPushMatrix ( ) ;
		glMultMatrix ( mTransformCurrent ) ;
		if ( ! toolList . empty ( ) )
		{
			glRotate ( mPositionOfCurrent, 0.0f, 1.0f, 0.0f ) ;
			glTranslate ( 0.0f, 0.0f, - mRadiusCurrent ) ;
			glRotate ( mRotationCurrent, 0.0f, 1.0f, 0.0f ) ;
			glPushMatrix ( ) ;
			float rotation ( 0 ) ;
			for ( ToolList::const_iterator pos ( toolList . begin ( ) ) ;
			      pos != toolList . end ( ) ; ++pos )
			{
				glPushMatrix ( ) ;
				glTranslate ( 0.0f, 0.0f, mRadiusCurrent ) ;
				ToolOffsetMap::const_iterator offsetPos ( mToolOffsets . find ( *pos ) ) ;
				if ( offsetPos != mToolOffsets . end ( ) ) {
					glTranslate ( 0.0f, - offsetPos -> second, offsetPos -> second / 2.0f ) ;
					glScale ( 1.0 + offsetPos -> second / mDetachOffset / 3.0 ) ;
				}
				glRotate ( rotation - mRotationCurrent - mPositionOfCurrent, 0.0f, 1.0f, 0.0f ) ;
				const float radiusRatio ( mRadiusCurrent / mRadius ) ;
				if ( mAlwaysShowCurrent )
				{
					glScale ( ( radiusRatio +
					            ( 1.0 - radiusRatio ) *
					              ( 1.0 - ( Math::abs ( minimumDegreeDistance ( mRotationCurrent, rotation )
					                                  ) / 180.0
					                      )
					            )
					          ) * mIconScale
					        ) ;
				}
				else
				{
					glScale ( radiusRatio * mIconScale ) ;
				}
				(*pos) -> icon ( ) -> display ( contextData ) ;
				glPopMatrix ( ) ;
				glRotate ( mThetaDegree, 0.0f, -1.0f, 0.0f ) ;
				rotation += mThetaDegree ;
			}
			glPopMatrix ( ) ;
		}
		glPopMatrix ( ) ;
	}
}

/*
 * Utilities
 */

Tool* ToolRotator::currentTool ( ) const
{
	if ( mCurrentToolListPosition == toolBox ( ) -> tools ( ) . end ( ) )
		return NULL ;
	else
		return *mCurrentToolListPosition ;
}

void ToolRotator::currentToolListPosition ( const ToolList::const_iterator& toolListPosition )
{
	if ( mCurrentToolListPosition != toolListPosition )
	{
		mCurrentToolListPosition = toolListPosition ;
		mTimeToClose = mCloseDelay ;
		if ( mOpenState == CLOSED || mOpenState == CLOSING )
			mOpenState = OPENING ;
		if ( mDetachedState == DETACHED && currentTool ( ) )
			mToolOffsets [ currentTool ( ) ] = 0.0f ;
		if ( mOpenState != CLOSED )
			mRotationTimeCurrent = mSwitchDelay ;
		update ( ) ;
	}
}

void ToolRotator::update ( )
{
	// Update the values used to draw and move the wheel
	mIconSpacing = mIconScale * 2.5 ;
	if ( toolBox ( ) -> tools ( ) . size ( ) > 1 )
	{
		const float thetaOverTwo ( Math::Constants<float>::pi / (float) toolBox ( ) -> tools ( ) . size ( ) ) ;
		mThetaDegree = Math::deg ( 2 * thetaOverTwo ) ;
		mRadius = mIconSpacing / ( 2 * Math::sin ( thetaOverTwo ) ) ;
		// Update the rotation goal
		mRotationGoal = 0 ;
		for ( ToolList::const_iterator pos ( toolBox ( ) -> tools ( ) . begin ( ) ) ;
		      pos != mCurrentToolListPosition ; ++pos )
		{ mRotationGoal += mThetaDegree ; }
		mDetachmentMagnitudeSqr = mIconSpacing * mIconSpacing / 4.0 ;
		mDetachOffset = mIconScale * 2.5 ;
	}
	else
	{
		mRadius = 1 ;
		mRotationCurrent = 0 ;
		mRotationGoal = 0 ;
		mDetachmentMagnitudeSqr = Math::Constants<float>::max ;
	}
}

void ToolRotator::moveToNextTool ( )
{
	const ToolList & tools ( toolBox ( ) -> tools ( ) ) ;
	if ( currentTool ( ) && tools . size ( ) > 1 )
	{
		ToolList::const_iterator next ( mCurrentToolListPosition ) ;
		++ next ;
		if ( next == tools . end ( ) )
			next = tools . begin ( ) ;
		currentToolListPosition ( next ) ;
	}
}

float ToolRotator::minimumDegreeDistance ( float from, float to )
{
	from = Math::mod ( from, 360.0f ) ;
	if ( from < 0.0f ) from += 360.0f ;
	to = Math::mod ( to, 360.0f ) ;
	if ( to < 0.0f ) to += 360.0f ;
	float distance ( to - from ) ;
	if ( distance > 180.0f )
		distance -= 360.0f ;
	else if ( distance < -180.0f )
		distance += 360.0f ;
	return distance ;
}

// Borrowed from http://ggt.sourceforge.net/html/group__Interp.html#a3
Vrui::Scalar * ToolRotator::lerp ( Vrui::Scalar * result, Vrui::Scalar t,
                                   const Vrui::Scalar * from, const Vrui::Scalar * to )
{
	// Just an alias to match q
	const Vrui::Scalar * p ( from ) ;
	
	// Calculate cosine theta
	Vrui::Scalar cosom (   from [ 0 ] * to [ 0 ]
	                     + from [ 1 ] * to [ 1 ]
	                     + from [ 2 ] * to [ 2 ]
	                     + from [ 3 ] * to [ 3 ] ) ;
	
	// Adjust signs (if necessary)
	Vrui::Scalar q [ 4 ] ;
	if ( cosom < Vrui::Scalar ( 0.0 ) )
	{
		q [ 0 ] = - to [ 0 ] ;   // Reverse all signs
		q [ 1 ] = - to [ 1 ] ;
		q [ 2 ] = - to [ 2 ] ;
		q [ 3 ] = - to [ 3 ] ;
	}
	else
	{
		q [ 0 ] = to [ 0 ] ;
		q [ 1 ] = to [ 1 ] ;
		q [ 2 ] = to [ 2 ] ;
		q [ 3 ] = to [ 3 ] ;
	}
	
	// Do linear interperolation
	Vrui::Scalar sclp ( Vrui::Scalar ( 1.0 ) - t );
	Vrui::Scalar sclq ( t ) ;
	result [ 0 ] = sclp * p [ 0 ] + sclq * q [ 0 ] ;
	result [ 1 ] = sclp * p [ 1 ] + sclq * q [ 1 ] ;
	result [ 2 ] = sclp * p [ 2 ] + sclq * q [ 2 ] ;
	result [ 3 ] = sclp * p [ 3 ] + sclq * q [ 3 ] ;
	return result;
}

} // namespace Extensions
} // namespace ToolBox
