/*******************************************************************************
 DataItem: OpenGL context data.
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
#include "DataItem.h"
#include <iostream>
// Vrui includes
//
#include <Misc/ThrowStdErr.h>
#include <GL/Extensions/GLARBVertexShader.h>
#include <GL/Extensions/GLARBFragmentShader.h>

// External includes
//
#include "VruiStreamManip.h"
#include "IO/ansi-color.h"

#include <string.h>

namespace DTS
{

//
// DataItem methods
//

DataItem::DataItem(void)
   : hasPointParameterExtension(GLARBPointParameters::isSupported()),
   hasVertexBufferObjectExtension(GLARBVertexBufferObject::isSupported()),
   hasShaders(GLARBShaderObjects::isSupported()&&GLARBVertexShader::isSupported()&&GLARBFragmentShader::isSupported()),
   vertexBufferId(0), spriteTextureObjectId(0), versionDS(0),
   versionPS(0),
   vertexShaderObject(0),fragmentShaderObject(0),programObject(0),
   numParticlesDS(0), numParticlesPS(0)
{
   master::filter masterout(std::cout);

   masterout() << "Initializing OpenGL extensions..." << std::endl;

   masterout() << "\tGL_ARB_POINT_PARAMETERS : ";
   if(hasPointParameterExtension)
   {
      GLARBPointParameters::initExtension();
      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   masterout() << "\tGL_ARB_VERTEX_BUFFER_OBJECT : ";
   if (hasVertexBufferObjectExtension)
   {
      // initialize the vertex buffer object extension
      GLARBVertexBufferObject::initExtension();

      // create a vertex buffer object
      glGenBuffersARB(1,&vertexBufferId);

      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   glGenTextures(1, &spriteTextureObjectId);

   masterout() << "\tGL_ARB_SHADER_OBJECTS : ";
   if(hasShaders)
   {
      /* Initialize the OpenGL extensions: */
      GLARBShaderObjects::initExtension();
      GLARBVertexShader::initExtension();
      GLARBFragmentShader::initExtension();

      /* Source code for vertex and fragment programs: */
      static const char* vertexProgram="\
         uniform float scaledParticleRadius; \
         \
         void main() \
         { \
         vec4 vertexEye; \
         \
         /* Transform the vertex to eye coordinates: */ \
         vertexEye=gl_ModelViewMatrix*gl_Vertex; \
         \
         /* Calculate point size based on vertex' eye distance along z direction: */ \
         gl_PointSize=scaledParticleRadius*2.0*vertexEye.w/vertexEye.z; \
         \
         /* Use standard vertex position for fragment generation: */ \
         gl_FrontColor=gl_Color; \
         gl_Position=ftransform(); \
         }";
      static const char* fragmentProgram="\
         uniform sampler2D tex0; \
         \
         void main() \
         { \
         gl_FragColor=texture2D(tex0,gl_TexCoord[0].xy)*gl_Color; \
         }";

      /* Compile and link the point size computation shader program: */
      vertexShaderObject=glCompileVertexShaderFromString(vertexProgram);
      fragmentShaderObject=glCompileFragmentShaderFromString(fragmentProgram);
      programObject=glLinkShader(vertexShaderObject,fragmentShaderObject);
      scaledParticleRadiusLocation=glGetUniformLocationARB(programObject,"scaledParticleRadius");
      tex0Location=glGetUniformLocationARB(programObject,"tex0");
      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   /* Display list for StaticSolverTool */
   dataDisplayListVersion = 0;
   dataDisplayListId=glGenLists(1);
}

DataItem::~DataItem(void)
{

    delete font;

   if(vertexBufferId>0)
   {
      // delete the vertex buffer object
      glDeleteBuffersARB(1,&vertexBufferId);
   }

   // delete texture object(s)
   glDeleteTextures(1, &spriteTextureObjectId);

   if(hasShaders)
   {
      glDeleteObjectARB(programObject);
      glDeleteObjectARB(vertexShaderObject);
      glDeleteObjectARB(fragmentShaderObject);
   }

   /* Display list for StaticSolverTool */
   glDeleteLists(dataDisplayListId, 1);

}

} // namspace::DTS
