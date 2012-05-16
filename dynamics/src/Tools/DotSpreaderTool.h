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
#ifndef DOT_SPREADER_TOOL_H
#define DOT_SPREADER_TOOL_H

// Vrui includes
//
#include <GL/GLFrustum.h>
#include <GL/GLModels.h>

// STL includes
//
#include <vector>
#include <iostream>

// GL includes
//
#include <GL/glu.h>

// Project includes
//
#include "FieldViewer.h"
#include "DataItem.h"
#include "ColorPoint.h"
#include "AbstractDynamicsTool.h"
#include "Dynamics/Vector.h"

#include "DotSpreaderOptionsDialog.h"

// Forward declarations
class DotSpreaderTool;

/** Data storage for DotSpreaderTool.
 *
 * DotSpreaderData contains all data relevant to the DotSpreaderTool. Specifically, it contains
 * the data necessary to initialize and render the point data.
 */
class DotSpreaderData
{
      friend class DotSpreaderTool;

   public:

      /// Initial particle distribution.
      enum Distribution
      {
         SURFACE, ///< Distribute particles on surface of sphere.
         VOLUME   ///< Distribute particles throughout volume of sphere.
      };

      typedef std::vector<ColorPoint> ParticleArray;
      typedef std::vector<DTS::Vector<double> > StateArray;

   private:
      ParticleArray particles;
      StateArray states;

      bool running;
      int numPoints;
      float point_radius;
      Distribution distribution;
      int dimension;

      unsigned int currentVersion;

      // numPoints(50000), point_radius(0.1),

      DotSpreaderData() :
         running(false), numPoints(10000), point_radius(0.05),
               distribution(SURFACE), dimension(0), currentVersion(0)
      {
      }

      ~DotSpreaderData()
      {
         particles.clear();
      }

      void setNumberOfParticles(int num)
      {
         particles.resize(num);

         if (num > numPoints)
         {
            for (int i=numPoints; i < num; i++)
            {
               particles.push_back(ColorPoint());
               states.push_back(DTS::Vector<double>(dimension));
            }
         }
         numPoints=num;
      }

      void init(int dimension)
      {
         this->dimension = dimension;

         for (int i=0; i < numPoints; i++)
         {
            particles.push_back(ColorPoint());
            states.push_back(DTS::Vector<double>(dimension));
         }
      }
};

/** Computes the trajectories of a large number of particles.
 *
 * The DotSpreaderTool allows the user to select a region of space by using the wand
 * to drag out a sphere. The user selects the origin of the sphere by pressing the
 * main button. The user selects varies the radius of the sphere by holding down the
 * button and moving the wand. When the button is released, the particles' positions
 * will be initialized based on the distribution method (see DotSpreaderData). There
 * are two ways particles can be distributed, on the surface or throughout the volume.
 * In either case, the particle's color is determined by its initial position. Each
 * particle is assigned a color similar to that of its neighbors. Thus, red and purple
 * particles were initially located close to one another. This coloring allows the
 * user to observe how much mixing there is in the system.
 */
class DotSpreaderTool: public AbstractDynamicsTool, public GLObject
{
   public:

      /* Embedded classes */

      class Icon: public ToolBox::Icon
      {
         public:
            Icon(const DotSpreaderTool* pTool) :
               parent(pTool)
            {
            }
            void display(GLContextData& contextData) const;
            const DotSpreaderTool* parent;
      };

      class DataItem: public GLObject::DataItem
      {
         public:
            DataItem();
            virtual ~DataItem();

            GLuint displayListId;
      };

      friend class Icon;
      friend class DataItem;

   public:

      /* Interface */

      DotSpreaderTool(ToolBox::ToolBox* toolBox, Viewer* app) :
         AbstractDynamicsTool(toolBox, app), dataInited(false),
         active(false), tempDisplay(3)
      {
         icon(new Icon(this));

         // Set member from parent class
         _needsGLSL = false;


      }

      virtual ~DotSpreaderTool()
      {
      }

      virtual void setExperiment(DTSExperiment* e)
      {
         experiment = e;
         tempState.setDimension( experiment->model->getDimension() );

         if (!dataInited)
         {
            data.init( experiment->model->getDimension() );
            dataInited = true;
         }

         // Start with a clean slate
         data.running = false;
      }

      void initContext(GLContextData& contextData) const;

      virtual void moved(const ToolBox::MotionEvent & motionEvent);
      virtual void mainButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent);
      virtual void mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent);
      virtual void otherButtonPressed(const ToolBox::ButtonPressEvent & buttonPressEvent)
      {
      }
      virtual void otherButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
      {
      }

      virtual void render(DTS::DataItem* dataItem) const;
      virtual void step();

      virtual CaveDialog* createOptionsDialog(GLMotif::PopupMenu *parent)
      {
         dialog=new DotSpreaderOptionsDialog(parent, this);
         return dialog;
      }

      /* New methods */

      void clearParticles()
      {
         data.running = false;
         data.currentVersion++;
      }

      void setNumberOfParticles(unsigned int num)
      {
         data.setNumberOfParticles(num);
      }

      void setDistributionMethod(DotSpreaderData::Distribution dist)
      {
         data.distribution=dist;
      }

      void setPointSize(float value)
      {
         data.point_radius=value;
      }

   private:
      DotSpreaderData data;
      bool dataInited;

      bool active;

      Vrui::Point pos;
      Vrui::Point org;
      DTS::Vector<double> tempDisplay;
      DTS::Vector<double> tempState;
};

#endif 	    /* !DOTSPREADERTOOL_H_ */
