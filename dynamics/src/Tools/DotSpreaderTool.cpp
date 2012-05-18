/*******************************************************************************
 DotSpreaderTool: Dot spreader dynamics tool.
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
#include "DotSpreaderTool.h"

// Vrui includes
//
#include <GL/Extensions/GLARBVertexShader.h>
#include <GL/Extensions/GLARBFragmentShader.h>
#include <GL/GLMaterial.h>
#include <GL/GLModels.h>

// OpenGL includes
//
#include <GL/glu.h>

// External includes
//
#include "VruiStreamManip.h"

//
// DotSpreaderTool::Icon methods
//
void DotSpreaderTool::Icon::display(GLContextData& contextData) const
{
   DataItem* dataItem=contextData.retrieveDataItem<DataItem> (parent);
   glCallList(dataItem->displayListId);
}

//
// DotSpreaderTool::DataItem methods
//

DotSpreaderTool::DataItem::DataItem(void) :
   displayListId(glGenLists(1))
{
}

DotSpreaderTool::DataItem::~DataItem(void)
{
   glDeleteLists(displayListId, 1);
}

//
// DotSpreaderTool methods
//

void DotSpreaderTool::initContext(GLContextData& contextData) const
{
   DataItem* dataItem=new DataItem;
   contextData.addDataItem(this, dataItem);

   // calculate and store positions of particles
   const int SIZE=12;
   float points[SIZE][3];

   srand(1234);
   for (int i=0; i < SIZE; i++)
   {
      points[i][0]=0.8 - 1.6 * ((float) rand() / (float) RAND_MAX);
      points[i][1]=0.8 - 1.6 * ((float) rand() / (float) RAND_MAX);
      points[i][2]=0.8 - 1.6 * ((float) rand() / (float) RAND_MAX);
   }

   // create a new display list
   glNewList(dataItem->displayListId, GL_COMPILE);

   GLUquadric* quadric=gluNewQuadric();

   // save current attribute state
   glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);

   // draw sphere wireframe sphere
   glDisable(GL_LIGHTING);

   gluQuadricDrawStyle(quadric, GLU_LINE);

   glColor3f(0.0f, 0.8f, 1.0f);

   gluSphere(quadric, 1.0, 8, 12);

   // set up materials/lighting for particles
   glEnable(GL_LIGHTING);

   GLMaterial
         sphereMaterial(GLMaterial::Color(1.0, 0.0, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   glMaterial(GLMaterialEnums::FRONT_AND_BACK, sphereMaterial);

   // draw randomly positioned spheres
   for (int i=0; i < SIZE; i++)
   {
      glPushMatrix();
      glTranslatef(points[i][0], points[i][1], points[i][2]);
      glDrawSphereIcosahedron(0.1, 8);
      glPopMatrix();
   }

   gluDeleteQuadric(quadric);

   // restore previous attribute state
   glPopAttrib();

   glEndList();
}

void DotSpreaderTool::render(DTS::DataItem* dataItem) const
{
   // if dragging locator render release sphere
   if (active and !data.running)
   {
      // save current attribute state
      #ifdef MESA
      // GL_POINT_BIT causes GL enum error
      glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      #else
      glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_POINT_BIT);
      #endif

      glDisable(GL_LIGHTING);

      // save current location
      glPushMatrix();

      // move to center of sphere
      glTranslatef(org[0], org[1], org[2]);

      // compute radius of sphere
      float radius=Math::sqrt((pos[0] - org[0]) * (pos[0] - org[0]) + (pos[1]
            - org[1]) * (pos[1] - org[1]) + (pos[2] - org[2]) * (pos[2]
            - org[2]));

      GLUquadricObj *quadric=gluNewQuadric();

      // draw transparent sphere
      gluQuadricDrawStyle(quadric, GLU_FILL);
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glColor4f(0.0f, 0.6f, 1.0f, 0.2f);

      gluSphere(quadric, radius, 10, 15);
      glDisable(GL_BLEND);

      // draw surrounding wireframe (solid)
      gluQuadricDrawStyle(quadric, GLU_LINE);
      glDepthMask(GL_TRUE);
      glColor4f(0.0f, 0.8f, 1.0f, 1.0f);

      gluSphere(quadric, radius, 10, 15);
      glDepthMask(GL_FALSE);

      gluDeleteQuadric(quadric);

      // restore previous position
      glPopMatrix();
      // restore previous attribute state
      glPopAttrib();
   }
   // if simulation is running draw particles
   else if (data.running)
   {
      // save current attribute state
      #ifdef MESA
      // GL_POINT_BIT causes GL enum error
      glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      #else
      glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_POINT_BIT);
      #endif

      glDisable(GL_LIGHTING);
      glDepthMask(GL_FALSE);

      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, dataItem->spriteTextureObjectId);
      glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
      glEnable(GL_POINT_SPRITE_ARB);

      float particleRadius=data.point_radius;

      // Query the OpenGL viewing frustum
      GLFrustum<float> frustum;
      frustum.setFromGL();

      #ifdef GHETTO
      if (0)
      #else
      if (dataItem->hasShaders)
      #endif
      {
         /* Calculate the scaled point size for this frustum: */
         GLfloat scaledParticleRadius=frustum.getPixelSize() * particleRadius
               / frustum.getEyeScreenDistance();

         /* Enable the vertex/fragment shader: */
         glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
         glUseProgramObjectARB(dataItem->programObject);
         glUniform1fARB(dataItem->scaledParticleRadiusLocation, scaledParticleRadius);
         glUniform1iARB(dataItem->tex0Location, 0);
      }
      else
      {
         glPointSize(particleRadius
               * Vrui::getNavigationTransformation().getScaling());

         GLfloat linear[3]= { 0.0, 1 / 5.0, 0.0 };
         glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, linear);

         //
         // NOTE::the following scaling calculation does not work properly
         // in the CAVE. This is due to changes in the OpenGL library and
         // cannot be fixed. On a 2D screen the scaling should look correct.
         //

         // Query the OpenGL viewing frustum
         GLFrustum<float> frustum;
         frustum.setFromGL();

         // Calculate the nominal pixel size for particles
         float pointSizeCounter=frustum.getPixelSize() * 2.0f * particleRadius;

         float pointSizeDenominator=frustum.getEyeScreenDistance();

         // Query the maximum point size accepted by glPointSize(...)
         GLfloat pointSizeRange[2];
         glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, pointSizeRange);

         // Enable point parameters
         glPointSize(pointSizeRange[1]); // select the maximum point size
         pointSizeCounter/=pointSizeRange[1]; // adjust the point size numerator
         GLfloat attenuation[3]= { 0.0f, 0.0f, 0.0f };
         attenuation[2]=Math::sqr(pointSizeDenominator / pointSizeCounter);
         glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, attenuation);
      }

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, dataItem->vertexBufferId);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      // If data has been modified, send to graphics card
      if (dataItem->versionDS != data.currentVersion)
      {
         dataItem->numParticlesDS = data.numPoints;
         glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataItem->numParticlesDS
               * sizeof(ColorPoint), &data.particles[0], GL_DYNAMIC_DRAW_ARB);

         dataItem->versionDS = data.currentVersion;
      }

      glInterleavedArrays(GL_C4UB_V3F, sizeof(ColorPoint), 0);

      glDrawArrays(GL_POINTS, 0, dataItem->numParticlesDS);

      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);

      #ifndef GHETTO
      if (dataItem->hasShaders)
      #endif
      {
         glUseProgramObjectARB(0);
         glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
      }

      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_POINT_SPRITE_ARB);
      glDisable(GL_BLEND);

      // restore previous attribute state
      glPopAttrib();
   }
}

void DotSpreaderTool::step()
{
   // exit if simulation is paused (dragging release sphere)
   if (!data.running)
      return;

   for (int i=0; i < data.numPoints; i++)
   {
      experiment->integrator->step(data.states[i], tempState);
      data.states[i] += tempState;
      experiment->transformer->transform(data.states[i], tempDisplay);
      data.particles[i].pos[0] = tempDisplay[0];
      data.particles[i].pos[1] = tempDisplay[1];
      data.particles[i].pos[2] = tempDisplay[2];
   }

   data.currentVersion++;
}

void DotSpreaderTool::moved(const ToolBox::MotionEvent & motionEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }
   // get current locator position and save value
   pos=toolBox()->deviceTransformationInModel().getOrigin();
}

void DotSpreaderTool::mainButtonPressed(const ToolBox::ButtonPressEvent & motionEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }

   // get current locator position and set origin
   org=toolBox()->deviceTransformationInModel().getOrigin();

   // pause simulation (integration)
   data.running=false;

   // set active (dragging) flag
   active=true;
}

void DotSpreaderTool::mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }

   master::filter(std::cout)() << "DotSpreader::releasing " << data.numPoints << " particles... "
         << std::endl;

   // get current locator position
   pos=toolBox()->deviceTransformationInModel().getOrigin();

   // compute radius of release sphere
   float radius=Math::sqrt((pos[0] - org[0]) * (pos[0] - org[0]) + (pos[1]
         - org[1]) * (pos[1] - org[1]) + (pos[2] - org[2]) * (pos[2] - org[2]));

   releaseParticles(pos, radius);
}


void DotSpreaderTool::releaseParticles(Vrui::Point pos, Vrui::Scalar radius)
{
   // release particles distributed within sphere
   double x, y, z;

   double xMin=pos[0] - radius;
   double xMax=pos[0] + radius;
   double yMin=pos[1] - radius;
   double yMax=pos[1] + radius;
   double zMin=pos[2] - radius;
   double zMax=pos[2] + radius;
   double deltaX=xMax - xMin;
   double deltaY=yMax - yMin;
   double deltaZ=zMax - zMin;

   if (data.distribution == DotSpreaderData::SURFACE)
   {
      // distribute particles on surface of sphere
      for (int i=0; i < data.numPoints; i++)
      {

         double u=(double) rand() / (double) RAND_MAX * 2.0 * radius - radius;
         double theta=(double) rand() / (double) RAND_MAX * 2.0 * M_PI;

         x=Math::sqrt(radius * radius - u * u) * Math::cos(theta);
         y=Math::sqrt(radius * radius - u * u) * Math::sin(theta);
         z=u;

         x+=pos[0];
         y+=pos[1];
         z+=pos[2];

         data.particles[i].pos[0]=x;
         data.particles[i].pos[1]=y;
         data.particles[i].pos[2]=z;

         tempDisplay[0] = x;
         tempDisplay[1] = y;
         tempDisplay[2] = z;
         experiment->transformer->invTransform(tempDisplay, data.states[i]);

         data.particles[i].color[0]=(unsigned int) (((x - xMin) / deltaX)
               * 255.0);
         data.particles[i].color[1]=(unsigned int) (((y - yMin) / deltaY)
               * 255.0);
         data.particles[i].color[2]=(unsigned int) (((z - zMin) / deltaZ)
               * 255.0);
         data.particles[i].color[3]=255;
      }
   }
   else if (data.distribution == DotSpreaderData::VOLUME)
   {
      // distribute particles within the sphere
      for (int i=0; i < data.numPoints; i++)
      {

         do
         {
            x=((double) rand() / (double) RAND_MAX) * 2.0 * radius - radius;
            y=((double) rand() / (double) RAND_MAX) * 2.0 * radius - radius;
            z=((double) rand() / (double) RAND_MAX) * 2.0 * radius - radius;
         } while (x * x + y * y + z * z > radius * radius);

         x+=pos[0];
         y+=pos[1];
         z+=pos[2];

         data.particles[i].pos[0]=x;
         data.particles[i].pos[1]=y;
         data.particles[i].pos[2]=z;

         tempDisplay[0] = x;
         tempDisplay[1] = y;
         tempDisplay[2] = z;
         experiment->transformer->invTransform(tempDisplay, data.states[i]);

         data.particles[i].color[0]=(unsigned int) (((x - xMin) / deltaX)
               * 255.0);
         data.particles[i].color[1]=(unsigned int) (((y - yMin) / deltaY)
               * 255.0);
         data.particles[i].color[2]=(unsigned int) (((z - zMin) / deltaZ)
               * 255.0);
         data.particles[i].color[3]=255;
      }
   }

   // turn off active (dragging) flag
   active=false;
   // resume simulation (integration)
   data.running=true;
}
