/*******************************************************************************
 DynamicsSolverTool: Dynamic solver dynamics tool.
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
#include "DynamicSolverTool.h"

// Vrui includes
//
#include <GL/GLModels.h>
#include <GL/GLFrustum.h>
#include <GL/GLModels.h>

// OpenGL includes
//
#include <GL/gle.h>
#include <GL/glu.h>

// STL includes
//
#include <algorithm>

//
// DynamicSolverTool::Icon methods
//

void DynamicSolverTool::Icon::display(GLContextData& contextData) const
{
   DataItem* dataItem=contextData.retrieveDataItem<DataItem> (parent);
   glCallList(dataItem->displayListId);
}

//
// DynamicSolverTool methods
//

void DynamicSolverTool::initContext(GLContextData& contextData) const
{
   DataItem* dataItem=new DataItem;
   contextData.addDataItem(this, dataItem);

   // set up meterial properties
   GLMaterial sphereMaterial(GLMaterial::Color(1.0, 0.0, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   GLMaterial cylinderMaterial(GLMaterial::Color(0.0, 0.8, 1.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   // create new display list
   glNewList(dataItem->displayListId, GL_COMPILE);

   GLUquadric* quadric=gluNewQuadric();

   // save current attribute state
   glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_LIGHTING);

   // draw sphere
   gluQuadricDrawStyle(quadric, GLU_FILL);

   glMaterial(GLMaterialEnums::FRONT_AND_BACK, sphereMaterial);

   // move a little to the left and draw sphere
   glPushMatrix();
   glTranslatef(-0.25f, 0.0, 0.0f);
   glDrawSphereIcosahedron(0.5, 12);
   glPopMatrix();

   // draw arrow
   glMaterial(GLMaterialEnums::FRONT_AND_BACK, cylinderMaterial);

   // draw a cylinder (arrow shaft) through the sphere
   glPushMatrix();
   glTranslatef(-1.0f, 0.0f, 0.0f);
   glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
   gluCylinder(quadric, 0.15, 0.15, 1.5, 8, 8);

   // move forward and draw arrow head
   glTranslatef(0.0f, 0.0f, 1.5f);
   gluCylinder(quadric, 0.30, 0.0, 0.5, 8, 8);

   glPopMatrix();

   // restore previous attribute state
   glPopAttrib();

   glEndList();
}

void DynamicSolverTool::render(DTS::DataItem* dataItem) const
{
   // draw lines
   if (data.lineStyle == DynamicSolverData::BASIC)
      drawBasicLines();
   else if (data.lineStyle == DynamicSolverData::POLYLINE)
      drawPolylines();

   // draw heads
   if (data.headStyle == DynamicSolverData::POINT)
      drawPointHeads(dataItem);
   else if (data.headStyle == DynamicSolverData::SPHERE)
      drawSphereHeads();
}

void DynamicSolverTool::step()
{
   DTS::Vector<double> prev( experiment->model->getDimension() );
   
   // We use a temporary with preallocated space.
   DTS::Vector<double> temp( prev );

   // for each point array (line)
   for (Data::MultiPointArray::iterator pointSet=data.points.begin(); pointSet
         != data.points.end(); ++pointSet)
   {
      // for each point in line
      for (Data::PointArray::iterator point=(*pointSet).begin(); point
             != (*pointSet).end(); ++point)
      {
         // if point is the first value (head)
         if (point == (*pointSet).begin())
         {
            // save the position
            prev=(*point);
            // integrate and move point to next position
            experiment->integrator->step(*point, temp);            
            (*point) += temp;
         }
         // for all other points (tail) simply set to previous point
         else
         {
            // We avoid std::swap which uses the copy constructor.
            temp = (*point);
            (*point) = prev;
            prev = temp;
         }
      }
   }
}

void DynamicSolverTool::moved(const ToolBox::MotionEvent & motionEvent)
{
}

void DynamicSolverTool::mainButtonPressed(const ToolBox::ButtonPressEvent & motionEvent)
{
}

void DynamicSolverTool::mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
{
   // get current locator position
   pos=toolBox()->deviceTransformationInModel().getOrigin();
   DTS::Vector<double> position(3);
   position[0] = pos[0];
   position[1] = pos[1];
   position[2] = pos[2];

   // create a new static solution
   std::cout << "Position: " << position << std::endl;
   DTS::Vector<double> invPos( experiment->model->getDimension() );
   experiment->transformer->invTransform(position, invPos);
   std::cout << "invTransform: " << invPos << std::endl;
   std::cout << std::endl; 

   // create a new point array
   DynamicSolverData::PointArray array;

   // initialize the array (set values to locator position)
   for (unsigned int i=0; i < data.history_size; i++)
   {
      // makes a copy using copy constructor
      array.push_back(invPos);
   }
   
   // add to point array vector
   data.points.push_back(array);

   if (data.cluster_size > 1)
   {
      for (unsigned int i=1; i < data.cluster_size; i++)
      {
         DynamicSolverData::PointArray a;

         for (unsigned int j=0; j < data.history_size; j++)
         {
            a.push_back(invPos);
            a[j][0] += (float) rand() / (float) RAND_MAX * 0.1 - 0.05;
            a[j][1] += (float) rand() / (float) RAND_MAX * 0.1 - 0.05;
            a[j][2] += (float) rand() / (float) RAND_MAX * 0.1 - 0.05;
         }

         data.points.push_back(a);
      }
   }

}

//
// DynamicSolverTool internal methods
//

void DynamicSolverTool::drawBasicLines() const
{
   // save the current attribute state
   glPushAttrib(GL_LIGHTING_BIT);
   glDisable(GL_LIGHTING);

   DTS::Vector<double> tmp( 3 );

   if (data.colorStyle == DynamicSolverData::SOLID)
   {
      glColor3f(1.0, 0.0, 0.0);

      // for all lines
      for (unsigned int i=0; i < data.points.size(); i++)
      {
         glBegin(GL_LINES);
         // for all points in line
         for (unsigned int j=1; j < data.points[i].size(); j++)
         {
            experiment->transformer->transform(data.points[i][j-1], tmp);         
            glVertex3f(tmp[0], tmp[1], tmp[2]);
            experiment->transformer->transform(data.points[i][j], tmp);                     
            glVertex3f(tmp[0], tmp[1], tmp[2]);
         }
         glEnd();
      }
   }

   else if (data.colorStyle == DynamicSolverData::GRADIENT)
   {
      // for all lines
      for (unsigned int i=0; i < data.points.size(); i++)
      {
         glBegin(GL_LINES);
         // for all points in line
         for (unsigned int j=1; j < data.points[i].size(); j++)
         {
            int index=(int) ((float) j / (float) data.points[i].size() * 255.0);
            const float* color=data.colorMap->getColor(index);

            glColor3fv(color);

            experiment->transformer->transform(data.points[i][j-1], tmp);         
            glVertex3f(tmp[0], tmp[1], tmp[2]);
            experiment->transformer->transform(data.points[i][j], tmp);                     
            glVertex3f(tmp[0], tmp[1], tmp[2]);

         }
         glEnd();
      }
   }

   // restore the previous attribute state
   glPopAttrib();
}

void DynamicSolverTool::drawPolylines() const
{
   // allocate space for gle rendering
   gleDouble pts[data.history_size][3];
   float colors[data.history_size][3];

   // save the current attribute state
   glPushAttrib(GL_LIGHTING_BIT);

   if (data.colorStyle == DynamicSolverData::SOLID)
   {
      // set material properties
      glEnable(GL_LIGHTING);

      GLMaterial material(GLMaterial::Color(1.0, 0.8, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

      glMaterial(GLMaterialEnums::FRONT_AND_BACK, material);

   }
   else if (data.colorStyle == DynamicSolverData::GRADIENT)
   {
      glDisable(GL_LIGHTING);

      for (unsigned int i=0; i < data.history_size; i++)
      {
         int index=(int) ((float) i / (float) data.history_size * 255.0);
         const float* color=data.colorMap->getColor(index);

         colors[i][0]=color[0];
         colors[i][1]=color[1];
         colors[i][2]=color[2];
      }

   }

   DTS::Vector<double> tmp( 3 );

   // for all lines
   for (unsigned int i=0; i < data.points.size(); i++)
   {
      // for all points in line
      for (unsigned int j=0; j < data.history_size; j++)
      {
         // set up gle data
         experiment->transformer->transform(data.points[i][j], tmp);         
         
         pts[j][0]=tmp[0];
         pts[j][1]=tmp[1];
         pts[j][2]=tmp[2];
      }

      // render line as a generalized cylinder
      glePolyCylinder(data.history_size, // num points in polyline
      pts, // polyline vertices
      colors, // colors at polyline vertices
      data.point_radius); // radius of polycylinder
   }

   // restore the previous attribute state
   glPopAttrib();
}

void DynamicSolverTool::drawPointHeads(DTS::DataItem* dataItem) const
{
   // save current attribute state
   glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_POINT_BIT);

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

   if (dataItem->hasShaders)
   {
      /* Calculate the scaled point size for this frustum: */
      GLfloat scaledParticleRadius=frustum.getPixelSize() * particleRadius / frustum.getEyeScreenDistance();

      /* Enable the vertex/fragment shader: */
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
      glUseProgramObjectARB(dataItem->programObject);
      glUniform1fARB(dataItem->scaledParticleRadiusLocation, scaledParticleRadius);
      glUniform1iARB(dataItem->tex0Location, 0);
   }
   else
   {
      glPointSize(particleRadius * Vrui::getNavigationTransformation().getScaling());

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

   // set point color
   glColor4f(1.0, 0.8, 0.0, 1.0);

   DTS::Vector<double> tmp( 3 );

   // render points
   glBegin(GL_POINTS);
   for (unsigned int i=0; i < data.points.size(); i++)
   {
      experiment->transformer->transform(data.points[i][0], tmp);
      glVertex3f(tmp[0], tmp[1], tmp[2]);
   }
   glEnd();

   if (dataItem->hasShaders)
   {
      glUseProgramObjectARB(0);
      glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
   }

   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_POINT_SPRITE_ARB);

   // restore previous attribute state
   glPopAttrib();
}

void DynamicSolverTool::drawSphereHeads() const
{
   // save the current attribute state
   glPushAttrib(GL_LIGHTING_BIT);
   glEnable(GL_LIGHTING);

   // set material properties
   GLMaterial material(GLMaterial::Color(1.0, 0.0, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   glMaterial(GLMaterialEnums::FRONT_AND_BACK, material);

   DTS::Vector<double> tmp( experiment->model->getDimension() );

   // for all lines render the head as a sphere
   for (unsigned int i=0; i < data.points.size(); i++)
   {
      glPushMatrix();
      experiment->transformer->transform(data.points[i][0], tmp);      
      glTranslatef(tmp[0], tmp[1], tmp[2]);
      glDrawSphereIcosahedron(data.point_radius, 12);
      glPopMatrix();
   }

   // restore previous attribute state
   glPopAttrib();
}
