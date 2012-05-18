/*******************************************************************************
 ParticleSprayerTool: Particle sprayer dynamics tool.
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
#include "ParticleSprayerTool.h"

// OpenGL includes
//
#include <GL/glu.h>
#include <GL/GLMaterial.h>

//
// ParticleSprayerTool::Icon methods
//
void ParticleSprayerTool::Icon::display(GLContextData& contextData) const
{
   DataItem* dataItem=contextData.retrieveDataItem<DataItem> (parent);
   glCallList(dataItem->displayListId);
}

//
// ParticleSprayerTool methods
//


void ParticleSprayerTool::initContext(GLContextData& contextData) const
{

   DataItem* dataItem=new DataItem;
   contextData.addDataItem(this, dataItem);

   BlueRedColorMap colorMap;

   // calculate and store point positions
   const unsigned int SIZE=16;
   Vrui::Point pts[SIZE];

   const float ToRadians=M_PI / 180.0;

   // randomly distribute points within volume of a cone
   srand(10000);
   for (unsigned int i=0; i < SIZE; i++)
   {
      float fi=(float) rand() / (float) RAND_MAX * 30.0;
      float theta=(float) rand() / (float) RAND_MAX * 360.0;
      float r=(float) rand() / (float) RAND_MAX * 2.0;

      pts[i][0]=r * sin(fi * ToRadians) * cos(theta * ToRadians);
      pts[i][1]=r * sin(fi * ToRadians) * sin(theta * ToRadians);
      pts[i][2]=r * cos(fi * ToRadians);
   }

   // create new display list
   glNewList(dataItem->displayListId, GL_COMPILE);

   // save current attribute state
   glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_LIGHTING);

   for (unsigned int i=0; i < SIZE; i++)
   {
      // color points based on "height"
      int index=(int) (pts[i][2] / 2.0 * 255.0);
      const float* color=colorMap.getColor(index);

      GLMaterial
            sphereMaterial(GLMaterial::Color(color[0], color[1], color[2]), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

      glMaterial(GLMaterialEnums::FRONT_AND_BACK, sphereMaterial);

      // base size on "height" as well
      float radius=0.03 + 0.08 * (float) pts[i][2] / 2.0;

      glPushMatrix();
      glTranslatef(pts[i][0], pts[i][1], pts[i][2]);
      glDrawSphereIcosahedron(radius, 8);
      glPopMatrix();
   }

   // restore previous attribute state
   glPopAttrib();

   glEndList();
   
}

void ParticleSprayerTool::render(DTS::DataItem* dataItem) const
{
   // draw all emitter objects
   drawEmitters();

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

   float particleRadius=data.point_radius;//*1000.0;

   // Query the OpenGL viewing frustrum
   GLFrustum<float> frustum;
   frustum.setFromGL();   

   #ifdef GHETTO
   if (0)
   #else
   if (dataItem->hasShaders)
   #endif
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
      glPointSize(particleRadius
            * Vrui::getNavigationTransformation().getScaling());

      GLfloat linear[3]= { 0.0, 1 / 5.0, 0.0 };
      glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, linear);

      //
      // NOTE::the following scaling calculation does not work properly
      // in the CAVE. This is due to changes in the OpenGL library and
      // cannot be fixed. On a 2D screen the scaling should look correct.
      //

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

   if (dataItem->versionPS != data.currentVersion)
   {
      dataItem->numParticlesDS = data.particles.size();
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataItem->numParticlesDS * sizeof(PointParticle), &data.particles[0], GL_DYNAMIC_DRAW_ARB);
      dataItem->versionPS = data.currentVersion;
   }

   glInterleavedArrays(GL_C4UB_V3F, sizeof(PointParticle), 0);


   // Care needed if particle number has changed but we haven't updated buffer.
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

   // restore previous attribute state
   glPopAttrib();
}


void ParticleSprayerTool::setExperiment(DTSExperiment* e)
{
   experiment = e;
   clearParticles();
   clearEmitters();
   temp.setDimension( e->model->getDimension() );
   old.setDimension( e->model->getDimension() );
}

void ParticleSprayerTool::step()
{
   int dimension = experiment->model->getDimension();

   // iterator over all emitters and add particles to the simulation
   for (Data::PointArray::iterator emit=data.emitters.begin(); emit
         != data.emitters.end(); ++emit)
   {
      int cluster_size=data.cluster_size;
      float cluster_radius=data.cluster_radius;

      for (int i=0; i < cluster_size; i++)
      {
         float dx=cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);
         float dy=cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);
         float dz=cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);

         Geometry::Point<double,3> shift((*emit)[0] + dx, (*emit)[1] + dy, (*emit)[2] + dz);
         data.addParticle(PointParticle(shift, data.lifetime));

         tempDisplay[0] = (*emit)[0] + dx;
         tempDisplay[1] = (*emit)[1] + dy;
         tempDisplay[2] = (*emit)[2] + dz;
         experiment->transformer->invTransform(tempDisplay, temp);
         data.states.push_back( temp );

      }
   }

   // calculate color based on ratio of velocity to max. velocity
   //const float max_vel = 0.6*0.6;
   static float max_vel=0.0;
   static double previous_time=Vrui::getApplicationTime();

   double current_time=Vrui::getApplicationTime();
   bool check_max=false;
   float next_max=0.0;

   // if more than 3 seconds have elapsed set flag to check velocities
   if ((current_time - previous_time) > 3.0)
   {
      check_max=true;
      previous_time=current_time;
   }

   check_max=true;

   // iterate over all particles

   int i = 0;
   Data::ParticleArray::iterator particle=data.particles.begin();
   while ( particle != data.particles.end() )
   {
      // if particle has expired delete it
      if ((*particle).frame > (*particle).lifetime)
      {
         // if the particle is the last one in the array do simple pop
         if (particle == (data.particles.end() - 1))
         {
            data.particles.pop_back();
            data.states.pop_back();
            break;
         }
         // otherwise swap particle with end of array and then pop
         else
         {
            (*particle)=*(data.particles.end() - 1);
            data.particles.pop_back();

            data.states[i] = data.states[ data.states.size() - 1 ];
            data.states.pop_back();
         }
      }

      // save previous position of particle
      old = data.states[i];

      // compute new position and update particle
      experiment->integrator->step(data.states[i], temp);
      data.states[i] += temp;

      experiment->transformer->transform(data.states[i], tempDisplay);
      // implicit cast from double to float
      (*particle).pos[0] = tempDisplay[0];
      (*particle).pos[1] = tempDisplay[1];
      (*particle).pos[2] = tempDisplay[2];

      // compute the (squared) speed of the particle
      float speed = 0.0;
      for (int j = 0; j < dimension; j++)
      {
         speed += (data.states[i][j] - old[j]) * (data.states[i][j] - old[j]);
      }

      if (check_max)
      {
         next_max=(speed > next_max ? speed : next_max);
      }

      int index=(int) (sqrt(speed) / sqrt(max_vel) * 255.0);

      if (index > 255)
         index=255;
      if (index < 0)
         index=0;

      const float* cv=data.colorMap.getColor(index);

      // update particle color
      (*particle).color[0]=(unsigned char) (cv[0] * 255.0);
      (*particle).color[1]=(unsigned char) (cv[1] * 255.0);
      (*particle).color[2]=(unsigned char) (cv[2] * 255.0);

      // increment frame count
      (*particle).frame++;


      // Next loop
      ++particle;
      ++i;
   }

   if (check_max)
      max_vel=next_max;

   // update data version (now out of sync)
   data.currentVersion++;
}

void ParticleSprayerTool::moved(const ToolBox::MotionEvent & motionEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }
   
   // get current locator position
   pos=toolBox()->deviceTransformationInModel().getOrigin();

   // if spraying particles
   if (data.action == ParticleSprayerData::SPRAY_PARTICLES and active)
   {
      int cluster_size=data.cluster_size; // number of particles to emit
      float cluster_radius=data.cluster_radius; // amount of "spread"

      DTS::Vector<double> invPos( experiment->model->getDimension() );

      // add particles to the simulation
      for (int i=0; i < cluster_size; i++)
      {
         float dx = cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);
         float dy = cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);
         float dz = cluster_radius * ((float) rand() / (float) RAND_MAX * 2.0 - 1.0);

         Geometry::Point<double,3> shift(pos[0] + dx, pos[1] + dy, pos[2] + dz);
         data.addParticle(PointParticle(shift, data.lifetime));

         tempDisplay[0] = pos[0] + dx;
         tempDisplay[1] = pos[1] + dy;
         tempDisplay[2] = pos[2] + dz;

         experiment->transformer->invTransform(tempDisplay, invPos);
         data.states.push_back( invPos );
      }
   }

   // if moving an emitter
   else if (data.action == ParticleSprayerData::MOVE_EMITTER and active)
   {
      // move the selected emitter to current locator position
      *data.selectedEmitter=pos;
   }

   // finally, check if hovering over (within) an emitter
   else if ((data.action == ParticleSprayerData::MOVE_EMITTER or data.action
         == ParticleSprayerData::DELETE_EMITTER) and !active)
   {
      // for each emitter compute the distance from the locator
      for (Data::PointArray::iterator emit=data.emitters.begin(); emit
            != data.emitters.end(); ++emit)
      {
         Vrui::Point emit_pos=(*emit);

         double distance=sqrt((pos[0] - emit_pos[0]) * (pos[0] - emit_pos[0])
               + (pos[1] - emit_pos[1]) * (pos[1] - emit_pos[1]) + (pos[2]
               - emit_pos[2]) * (pos[2] - emit_pos[2]));

         // if locator lies within emitter set hovering emitter and return
         if (distance < data.emitter_radius)
         {
            data.hoveringEmitter=&*emit;
            return;
         }
      }

      // if not hovering inside emitter set to null
      data.hoveringEmitter=NULL;
   }
}

void ParticleSprayerTool::mainButtonPressed(const ToolBox::ButtonPressEvent & motionEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }


   // get current locator position
   pos=toolBox()->deviceTransformationInModel().getOrigin();

   if (data.action == ParticleSprayerData::CREATE_EMITTER)
      data.addEmitter(pos);

   else if (data.action == ParticleSprayerData::MOVE_EMITTER)
   {
      // for each emitter compute the distance from the locator
      for (Data::PointArray::iterator emit=data.emitters.begin(); emit
            != data.emitters.end(); ++emit)
      {
         Vrui::Point emit_pos=(*emit);

         double distance=sqrt((pos[0] - emit_pos[0]) * (pos[0] - emit_pos[0])
               + (pos[1] - emit_pos[1]) * (pos[1] - emit_pos[1]) + (pos[2]
               - emit_pos[2]) * (pos[2] - emit_pos[2]));

         // if locator lies within emitter set selected emitter and break
         if (distance < data.emitter_radius)
         {
            data.selectedEmitter=&*emit;
            break;
         }
      }
   }

   else if (data.action == ParticleSprayerData::DELETE_EMITTER)
   {
      // for each emitter compute the distance from the locator
      for (Data::PointArray::iterator emit=data.emitters.begin(); emit
            != data.emitters.end(); ++emit)
      {
         Vrui::Point emit_pos=(*emit);

         double distance=sqrt((pos[0] - emit_pos[0]) * (pos[0] - emit_pos[0])
               + (pos[1] - emit_pos[1]) * (pos[1] - emit_pos[1]) + (pos[2]
               - emit_pos[2]) * (pos[2] - emit_pos[2]));

         // if locator lies within emitter delete the emitter and return
         if (distance < data.emitter_radius)
         {
            data.emitters.erase(emit);
            return;
         }
      }
   }

   active=true;
}

void ParticleSprayerTool::mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
{
   if (experiment == NULL || locked)
   {
      return;
   }

   data.selectedEmitter=NULL;
   active=false;
}

//
// ParticleSprayerTool internal methods
//

void ParticleSprayerTool::drawEmitters() const
{
   GLMaterial basicEmitterMaterial(GLMaterial::Color(0.0, 0.5, 1.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   GLMaterial selectedEmitterMaterial(GLMaterial::Color(1.0, 0.0, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   GLMaterial hoveringEmitterMaterial(GLMaterial::Color(1.0, 0.5, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

   glMaterial(GLMaterialEnums::FRONT_AND_BACK, basicEmitterMaterial);

   // draw all emitters
   for (Data::PointArray::const_iterator emit=data.emitters.begin(); emit
         != data.emitters.end(); ++emit)
   {
      Vrui::Point pos=(*emit);

      glPushMatrix();
      glTranslatef(pos[0], pos[1], pos[2]);
      glDrawSphereIcosahedron(data.emitter_radius, 12);
      glPopMatrix();
   }

   // draw selected emitter if assigned
   if (data.selectedEmitter != NULL)
   {
      glMaterial(GLMaterialEnums::FRONT_AND_BACK, selectedEmitterMaterial);

      Vrui::Point pos=*(data.selectedEmitter);
      glPushMatrix();
      glTranslatef(pos[0], pos[1], pos[2]);
      glDrawSphereIcosahedron(data.emitter_radius, 12);
      glPopMatrix();
   }
   // otherwise draw hovering emitter if assigned
   else if (data.hoveringEmitter != NULL)
   {
      glMaterial(GLMaterialEnums::FRONT_AND_BACK, hoveringEmitterMaterial);

      Vrui::Point pos=*(data.hoveringEmitter);
      glPushMatrix();
      glTranslatef(pos[0], pos[1], pos[2]);
      glDrawSphereIcosahedron(data.emitter_radius, 12);
      glPopMatrix();
   }
}




