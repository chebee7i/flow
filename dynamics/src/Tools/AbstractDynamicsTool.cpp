/*******************************************************************************
 AbstractDynamicsTool: Base class for dynamics tools.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#include "AbstractDynamicsTool.h"

#include "FieldViewer.h"

void AbstractDynamicsTool::grabbed(const ToolBox::ToolGrabEvent & toolGrabEvent)
{
   setDisabled(false);
   //application->updateToolToggles();
   //application->updateCurrentOptionsDialog();
}
