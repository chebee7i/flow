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
#ifndef DYNAMIC_SOLVER_TOOL_H
#define DYNAMIC_SOLVER_TOOL_H

// STL includes
//
#include <vector>

// Vrui includes
//
#include <GL/GLModels.h>

// External includes
//
#include "ColorMap/ColorMap.h"

// Project includes
//
#include "DataItem.h"
#include "AbstractDynamicsTool.h"
#include "Dynamics/Vector.h"

#include "DynamicSolverOptionsDialog.h"

// Forward declarations
class DynamicSolverTool;

/** Data storage for DynamicSolverTool.
 *
 * DynamicSolverData contains all data relevant to the DynamicSolverTool.
 * Specifically, it stores the render options such as the line style,
 * line color, and point style as well as the point values which are used
 * to render the particles.
 *
 * Particles can be rendered either as single points/spheres or with a
 * tail showing a finite number of previous positions.
 */
class DynamicSolverData
{
      friend class DynamicSolverTool;

   public:
      enum LineStyle
      {
         NO_LINE, BASIC, POLYLINE
      };
      enum HeadStyle
      {
         NO_HEAD, POINT, SPHERE
      };
      enum ColorStyle
      {
         SOLID, GRADIENT
      };

   private:
      typedef std::vector<DTS::Vector<double> > PointArray;
      typedef std::vector<PointArray> MultiPointArray;

      MultiPointArray points; ///< Actual point data.

      LineStyle lineStyle; ///< Style used in rendering tail.
      HeadStyle headStyle; ///< Style used in rendering head (particle).
      ColorStyle colorStyle; ///< Color used in rendering tail.

      float point_radius; ///< Size of head (particle).
      unsigned int history_size; ///< Length of tail.
      unsigned int cluster_size; ///< Number of particles in simultaneous release mode.

      ColorMap* colorMap; ///< Color map for rendering color gradient.

      DynamicSolverData() :
         lineStyle(BASIC), headStyle(SPHERE), colorStyle(SOLID),
               point_radius(0.25), history_size(50), cluster_size(1)
      {
         colorMap=new BlueRedColorMap;
      }

      ~DynamicSolverData()
      {
         delete colorMap;
      }
};

/** Computes the paths for multiple particles and renders them dynamically
 *
 * The DynamicSolverTool continuously renders the positions of
 */
class DynamicSolverTool: public AbstractDynamicsTool, public GLObject
{
   public:

      /* Embedded classes */
      class Icon: public ToolBox::Icon
      {
         public:
            Icon(const DynamicSolverTool* pTool) :
               parent(pTool)
            {
            }
            void display(GLContextData& contextData) const;
            const DynamicSolverTool* parent;
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

   public:
      DynamicSolverTool(ToolBox::ToolBox* toolBox, Viewer* app) :
         AbstractDynamicsTool(toolBox, app)
      {
         icon(new Icon(this));

         // Set member from parent class
         _needsGLSL = false;

      }

      virtual ~DynamicSolverTool()
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
         dialog=new DynamicSolverOptionsDialog(parent, this);
         return dialog;
      }

      /* New methods */

      void clearPoints()
      {
         data.points.clear();
      }

      void setLineStyle(DynamicSolverData::LineStyle style)
      {
         data.lineStyle=style;
         Vrui::requestUpdate();
      }

      void setHeadStyle(DynamicSolverData::HeadStyle style)
      {
         data.headStyle=style;
         Vrui::requestUpdate();
      }

      void setColorStyle(DynamicSolverData::ColorStyle style)
      {
         data.colorStyle=style;
         Vrui::requestUpdate();
      }

      void setPointSize(float value)
      {
         data.point_radius=value;
      }

      void setReleaseClusterSize(unsigned int value)
      {
         data.cluster_size=value;
      }
   private:
      typedef DynamicSolverData Data;
      DynamicSolverData data;

      /* Internal methods */
      void drawBasicLines() const;
      void drawPolylines() const;

      void drawPointHeads(DTS::DataItem* dataItem) const;
      void drawSphereHeads() const;
};

#endif
