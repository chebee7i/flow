/*******************************************************************************
 DataItem: OpenGL context data.
 Copyright (c) 2006-2008 Jordan Van Aalsburg, Oliver Kreylos

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
#ifndef DATA_ITEM_H
#define DATA_ITEM_H

// Vrui includes
//
#include <GL/GLObject.h>
#include <GL/GLContextData.h>
#include <GL/GLExtensionManager.h>
#include <GL/Extensions/GLARBPointParameters.h>
#include <GL/Extensions/GLARBVertexBufferObject.h>
#include <GL/Extensions/GLARBShaderObjects.h>

// font rendering
#include <FTGL/ftgl.h>



namespace DTS
{

/** Data structure storing OpenGL-dependent application data.
 */
struct DataItem: public GLObject::DataItem
{
   public:
      bool hasPointParameterExtension; ///< Flag whether point parameters are supported.
      bool hasVertexBufferObjectExtension; ///< GFlag whether VBOs are supported.
      bool hasShaders; ///< Flag whether local OpenGL supports GLSL shaders.

      GLuint vertexBufferId; ///< Vertex object buffer ID.
      GLuint spriteTextureObjectId; ///< Texture object ID for point sprites.

      ///< Used for syncing VOB rendering.
      unsigned int versionDS;
      unsigned int versionPS;

      /* State for vertex / fragment shaders: */

      GLhandleARB vertexShaderObject, fragmentShaderObject, programObject; ///< Shader for proper point size attenuation
      GLint scaledParticleRadiusLocation; ///< Location of particle radius uniform variable in shader program
      GLint tex0Location; ///< Location of texture sample uniform variable in shader program
      int numParticlesDS; ///< Number of particles still alive at last step()
      int numParticlesPS; ///< Number of particles still alive at last step()


      /* Variables for StaticSolverTool */
      /*
         Note, each static solver tool must have its own id and version.
         If they share an id, then if you ever have two static solver tools,
         they will enter an endless loop:

         Initially, we might have:
           dataDisplayListVersion = 2
           staticSolverTool_1_DisplayListVersion = 2
           staticSolverTool_2_DisplayListVersion = 2

         After a click with the first static solver tool, we have:
           dataDisplayListVersion = 2
           staticSolverTool_1_DisplayListVersion = 3
           staticSolverTool_2_DisplayListVersion = 2

         When the first tool is rendered, we have:
           dataDisplayListVersion = 3
           staticSolverTool_1_DisplayListVersion = 3
           staticSolverTool_2_DisplayListVersion = 2

         When the second tool is rendered, we have:
           dataDisplayListVersion = 2
           staticSolverTool_1_DisplayListVersion = 3
           staticSolverTool_2_DisplayListVersion = 2

         And so, we see that the the effect the first tool must be rendered
         again, giving us an endless loop.  The same holds true for anything
         else where we must make use of an id and version. To avoid this,
         we will make the tool a singleton.
      */
      GLuint dataDisplayListId;
      unsigned int dataDisplayListVersion;

      // fonts
      FTFont* font;

      DataItem(void);
      virtual ~DataItem(void);
};

}

#endif
