/*******************************************************************************
 StaticSolverTool: Static solver dynamics tool.
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
#ifndef STATIC_SOLVER_TOOL_H
#define STATIC_SOLVER_TOOL_H

// STL includes
//
#include <vector>
#include <iostream>

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

#include "StaticSolverOptionsDialog.h"

// Forward declarations
class StaticSolverTool;
class GLPolylineTube;

/** Data storage for StaticSolverTool.
 *
 * StaticSolverData contains all data relevant to the StaticSolverTool.
 * Specifically, it stores render options such as the line style and
 * color as well as the point values which are used to render the
 * particle path.
 */
class StaticSolverData
{
      friend class StaticSolverTool;

   public:
      StaticSolverData(int modelDimension) :
         numberOfPoints(5000), lineStyle(BASIC), colorStyle(GRADIENT)
      {
         points.reserve(numberOfPoints);

         // initialize the array with default (0,0,0) point values
         for (unsigned int i=0; i < MaxPoints; i++)
            points.push_back(DTS::Vector<double>(modelDimension));

         // create the color map
         colorMap=new BlueRedColorMap;
      }

      ~StaticSolverData()
      {
         delete colorMap;
      }

      enum LineStyle
      {
         BASIC, POLY_LINE
      };

      enum ColorStyle
      {
         SOLID, GRADIENT
      };

      static const unsigned int MaxPoints;

   private:
      unsigned int numberOfPoints; ///< Number of points to use when rendering line.
      std::vector<DTS::Vector<double> > points; ///< Actual point data.

      LineStyle lineStyle; ///< Style used in rendering line.
      ColorStyle colorStyle; ///< Color used in redering line.

      ColorMap* colorMap; ///< Color map for rendering color gradient.
};

/** Computes the path of a particle and renders it as a line.
 *
 * The StaticSolverTool computes the particles trajectory and renders
 * it as a line. When the user presses the main button the dynamical
 * equations are solved taking the position of the wand/cursor as
 * the initial starting point. In this way the user can explore
 * the system and investigate the trajectory's dependence on the
 * initial position.
 *
 * There are a number of options associated with the rendering of
 * of the particle's path which are stored in the StaticSolverToolData
 * class. These values can be set by StaticSolverOptionsDialog.
 */
class StaticSolverTool: public AbstractDynamicsTool, public GLObject
{
   public:

      /* Embedded classes */

      class Icon: public ToolBox::Icon
      {
         public:
            Icon(const StaticSolverTool* pTool) :
               parent(pTool)
            {
            }

            void display(GLContextData& contextData) const;

            const StaticSolverTool* parent;
      };

      class DataItem: public GLObject::DataItem
      {
            friend class StaticSolverTool;
         public:
            DataItem()
            {
               iconDisplayListId=glGenLists(1);
            }
            virtual ~DataItem()
            {
               glDeleteLists(iconDisplayListId, 1);
            }

            GLuint iconDisplayListId;
      };

      friend class Icon;
      friend class DataItem;
      friend class StaticSolverOptionsDialog;

   public:

      /* Interface */

      StaticSolverTool(ToolBox::ToolBox* toolBox, Viewer* app);
      virtual ~StaticSolverTool();

      void initContext(GLContextData& contextData) const;
      void display(GLContextData& contextData) const;
      virtual void render(DTS::DataItem* dataItem) const;
      virtual void setExperiment(DTSExperiment* e);
      virtual void updatedExperiment();
      virtual void step();

      void addStaticSolution(DTS::Vector<double> position);

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
         dialog=new StaticSolverOptionsDialog(parent, this);
         return dialog;
      }

      /* New methods */

      void setColorStyle(StaticSolverData::ColorStyle style)
      {
         colorStyle = style;
         std::vector<StaticSolverData*>::iterator it;
         for (it = datasets.begin(); it != datasets.end(); it++)
         {
            (*it)->colorStyle = style;
         }
         requestDataDisplayListUpdate();
         Vrui::requestUpdate();
      }

      void setLineStyle(StaticSolverData::LineStyle style)
      {
         lineStyle = style;
         std::vector<StaticSolverData*>::iterator it;
         for (it = datasets.begin(); it != datasets.end(); it++)
         {
            (*it)->lineStyle = style;
         }
         requestDataDisplayListUpdate();
         Vrui::requestUpdate();
      }

      void setNumberOfPoints(unsigned int size)
      {
         StaticSolverData* data;
         std::vector<StaticSolverData*>::iterator it;
         DTS::Vector<double> step(experiment->model->getDimension());

         for (it = datasets.begin(); it != datasets.end(); it++)
         {
            data = *it;

            if (size > data->numberOfPoints)
            {
               //data->points.resize(size);
               // can't resize since dimension must be known at initialization
               // so we push_back as we update...
               for (unsigned int i=data->numberOfPoints; i < size; i++)
               {
                  data->points.push_back( data->points[i-1] );
                  experiment->integrator->step(data->points[i-1], step);
                  data->points[i] += step;
               }
            }

            // Update the size no matter what
            (*it)->numberOfPoints = size;
         }

         requestDataDisplayListUpdate();
         Vrui::requestUpdate();
      }

   private:
      /* Internal variables */
      std::vector<StaticSolverData*> datasets; ///< Container for pointers to dynamically allocated StaticSolverData instances.
      unsigned int dataDisplayListVersion;
      bool multipleStaticSolutions;

      // Store current values so we can reset to save values after clearing.
      StaticSolverData::LineStyle lineStyle;
      StaticSolverData::ColorStyle colorStyle;

      /* Internal methods */
      void computeStaticSolution(StaticSolverData* d);
      void clearDatasets();
      void drawBasicLine(StaticSolverData* d) const;
      void drawPolyLine(StaticSolverData* d) const;
      void requestDatasetsUpdate();
      void requestDataDisplayListUpdate();
      void updateDataDisplayList(DTS::DataItem* dataItem) const;
};

#endif
