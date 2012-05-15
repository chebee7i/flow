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
#ifndef PARTICLE_SPRAYER_TOOL_H
#define PARTICLE_SPRAYER_TOOL_H

// STL includes
//
#include <vector>
#include <iostream>

// Vrui includes
//
#include <Vrui/Vrui>
#include <GL/GLFrustum.h>
#include <GL/GLModels.h>
#include <GL/GLMaterial.h>

// External includes
//
#include "ColorMap/ColorMap.h"

// Project includes
//
#include "DataItem.h"
#include "PointParticle.h"
#include "AbstractDynamicsTool.h"
#include "Dynamics/Vector.h"

#include "ParticleSprayerOptionsDialog.h"

// Forward declarations
class ParticleSprayerTool;

/** Data storage for ParticleSprayerTool.
 *
 * ParticleSprayerData contains all data relevant to the ParticleSprayerTool.
 * Specifically, it stores the positions of all the particles in the system
 * as well as the positions and types of emitter objects.
 */
class ParticleSprayerData
{
      friend class ParticleSprayerTool;

      typedef std::vector<PointParticle> ParticleArray;
      typedef std::vector<Vrui::Point> PointArray;
      typedef std::vector<DTS::Vector<double> > StateArray;

   public:
      /// Various sprayer actions.
      enum Action
      {
         SPRAY_PARTICLES, CREATE_EMITTER, MOVE_EMITTER, DELETE_EMITTER
      };

   private:
      ParticleArray particles; ///< Point particles.
      PointArray emitters; ///< Location of particle emitters.
      StateArray states; ///< Particle state variables (in n-dimensions).
      
      Action action; ///< Current sprayer action (mode).

      Vrui::Point* selectedEmitter;
      Vrui::Point* hoveringEmitter;

      int cluster_size; ///< Number of particles to emit.
      float cluster_radius; ///< Spread of particles emitted.
      unsigned int lifetime; ///< Lifetime of particles.
      float emitter_radius; ///< Size of spheres representing emitters.
      float point_radius; ///< Size of the particles.
      
      unsigned int currentVersion; ///< For syncing VOB rendering.

      BlueRedColorMap colorMap; ///< Color map for coloring by velocity.

      ParticleSprayerData() :
         action(SPRAY_PARTICLES), selectedEmitter(NULL), hoveringEmitter(NULL),
         cluster_size(15), cluster_radius(0.5), lifetime(750),
         emitter_radius(0.1), point_radius(0.05), currentVersion(0)
         
      {
         particles.reserve(200000);         
      }

      ~ParticleSprayerData()
      {
         //particles.clear();
      }

      /** Add an emitter at specified position.
       */
      void addEmitter(const Vrui::Point& p)
      {
         emitters.push_back(p);
      }

      /** Add a particle at specified position.
       */
      void addParticle(const PointParticle& p)
      {
         particles.push_back(p);
      }
};

/** Emits particles with a finite lifetime.
 *
 * The motivation behind the ParticleSprayerTool comes from wind tunnel
 * tests where smoke is used to probe the aerodynamic properties of a
 * model. Here, the user is able to introduce particles into the system
 * which behave in a similar way. Because the particles have a finite
 * lifetime, they will not "clutter" the space. The color of each particle
 * is determined by its instantaneous velocity.
 */
class ParticleSprayerTool: public AbstractDynamicsTool, public GLObject
{
   public:

      /* Embedded classes */

      class Icon: public ToolBox::Icon
      {
         public:
            Icon(const ParticleSprayerTool* pTool) :
               parent(pTool)
            {
            }
            void display(GLContextData& contextData) const;
            const ParticleSprayerTool* parent;
      };

      class DataItem: public GLObject::DataItem
      {
         public:
            DataItem()
            {
               displayListId=glGenLists(1);
            }
            virtual ~DataItem()
            {
               glDeleteLists(displayListId, 1);
            }

            GLuint displayListId;
      };

      friend class Icon;
      friend class DataItem;

      /* Interface */

      ParticleSprayerTool(ToolBox::ToolBox* toolBox, Viewer* app) :
         AbstractDynamicsTool(toolBox, app), active(false)
      {
         icon(new Icon(this));

         // Set member from parent class
         _needsGLSL = false;

      }

      virtual ~ParticleSprayerTool()
      {
      }

      void initContext(GLContextData& contextData) const;
      virtual void render(DTS::DataItem* dataItem) const;
      virtual void step();

      virtual void moved(const ToolBox::MotionEvent & motionEvent);
      virtual void mainButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent);
      virtual void mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent);
      virtual void otherButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent)
      {
      }
      virtual void otherButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
      {
      }

      virtual CaveDialog* createOptionsDialog(GLMotif::PopupMenu *parent)
      {
         dialog=new ParticleSprayerOptionsDialog(parent, this);
         return dialog;
      }

      /* New methods */

      /** Set the particle sprayer action.
       */
      void setAction(ParticleSprayerData::Action action)
      {
         data.action=action;
      }

      /** Delete all particles.
       */
      void clearParticles()
      {
         data.particles.clear();
         data.currentVersion++;
      }

      /** Delete all emitter objects.
       */
      void clearEmitters()
      {
         data.emitters.clear();
      }

      /** Set the particle lifetime (length of particle stream)
       */
      void setLifetime(unsigned int lt)
      {
         data.lifetime=lt;
      }

      /** Set distance between emitted particles.
       */
      void setEmitterSpread(float value)
      {
         data.cluster_radius=value;
      }

      void setPointSize(float value)
      {
         data.point_radius=value;
      }

   private:
      typedef ParticleSprayerData Data;

      bool active;
      ParticleSprayerData data;

      /* Internal methods */
      void drawEmitters() const;
};

#endif
