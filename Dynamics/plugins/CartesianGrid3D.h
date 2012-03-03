 
#ifndef CARTESIANGRID3D_INCLUDED_
#define CARTESIANGRID3D_INCLUDED_

#include <iostream>

#include "TrilinearInterpolator.h"
#include "BaseModel.h"
#include "RungeKutta4.h"

#include "CartesianGrid3DParameterDialog.h"

#include <math.h>
#include "boost/multi_array.hpp"

class CartesianGrid3D : public DynamicalModel
{
public:
    typedef boost::multi_array<Vector, 3> grid_type;
    typedef grid_type::index index;

protected:
    grid_type& grid;
        
    Point firstPosition;
    Point lastPosition;

public:
    CartesianGrid3D(grid_type& grid, Point& gridSpacing, Point& firstPosition)
        : DynamicalModel(),
          grid(grid)
    { 
        simulateGrid = true;
        
        this->firstPosition = firstPosition;
        
        gridOffset = Point(firstPosition[0] - floor( firstPosition[0] / gridSpacing[0] ),
                           firstPosition[1] - floor( firstPosition[1] / gridSpacing[1] ),
                           firstPosition[2] - floor( firstPosition[2] / gridSpacing[2] ));

        lastPosition = Point(firstPosition[0] + (grid.shape()[0] - 1) * gridSpacing[0],
                             firstPosition[1] + (grid.shape()[1] - 1) * gridSpacing[1],
                             firstPosition[2] + (grid.shape()[2] - 1) * gridSpacing[2]);
    }

    virtual ~CartesianGrid3D() { }

    Vector exact(const Point& p) const
    {
        //std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;

        /** The expectation is that whenever the Interpolator calls exact()
            that it should be seeking to evaluate *on* the grid.  So we allow
            ourselves a little bit of room (half the gridspacing) for numerical
            roundoff errors.
        **/     
        if (p[0] < firstPosition[0] - gridSpacing[0]/2.0 || 
            p[0] >  lastPosition[0] + gridSpacing[0]/2.0 ||
            p[1] < firstPosition[1] - gridSpacing[0]/2.0 || 
            p[1] >  lastPosition[1] + gridSpacing[0]/2.0 || 
            p[2] < firstPosition[2] - gridSpacing[0]/2.0 || 
            p[2] >  lastPosition[2] + gridSpacing[0]/2.0)
        {
            // Then the point is definitely outside the grid.
            // extrapolation...turn the vector field off.
            return Vector(0,0,0);
        }
        else
        {
            /** Otherwise, we need to return the value of the point on the grid
                nearest to the requested point. So our goal is to convert the 
                floating point to the proper index values.
            **/
            std::vector<int> index(3);
            index[0] = round((p[0]-firstPosition[0])/gridSpacing[0]);
            index[1] = round((p[1]-firstPosition[1])/gridSpacing[1]);
            index[2] = round((p[2]-firstPosition[2])/gridSpacing[2]);

            if (index[0] < 0 || index[0] > (int)grid.shape()[0] ||
                index[1] < 0 || index[1] > (int)grid.shape()[1] ||
                index[2] < 0 || index[2] > (int)grid.shape()[2] )
            {
                std::cout << "Bad indexes for grid." << std::endl;
                return Vector(0,0,0);
            }
            else
            {
                return grid[index[0]][index[1]][index[2]];
            }
        }
    }

    Point center()
    {
        return firstPosition + (lastPosition - firstPosition) / 2.0;
    }

private:
  virtual void _setValue(const std::string& name, Scalar value)
    {
    }

};

class CartesianGrid3DIntegrator : public Integrator
{
public:
  // public for now
  CartesianGrid3D* dynamics;
  RungeKutta4<CartesianGrid3D>* integrator;
  CartesianGrid3D::grid_type* grid;


  CartesianGrid3DIntegrator()
    {
      boost::array<CartesianGrid3D::index, 3> shape = {{ 10, 10, 10 }};
      grid = new CartesianGrid3D::grid_type(shape);
      Point gridSpacing(1,1,1);
      Point firstPosition(-20,-20,-20);

      /** lorentz **/

      typedef CartesianGrid3D::index index;
      Scalar x,y,z;
      Scalar sigma = 10;
      Scalar r = 21;
      Scalar b = 2.6;
      for(index i = 0; i != shape[0]; ++i) 
      {
        x = firstPosition[0] + gridSpacing[0] * i;
        for(index j = 0; j != shape[1]; ++j)
        {
          y = firstPosition[1] + gridSpacing[1] * j;
          for(index k = 0; k != shape[2]; ++k)
          {
            z = firstPosition[2] + gridSpacing[2] * k;
            (*grid)[i][j][k] = Vector(sigma*(y-x), r*x-y-x*z, x*y-b*z);
          }
        }
      }  
/**
      typedef CartesianGrid3D::index index;
      Scalar x,y,z;
      Scalar s = .12;
      Scalar dk_Y = -3;
      Scalar dk_XZ = 0;
      for(index i = 0; i != shape[0]; ++i) 
      {
        x = firstPosition[0] + gridSpacing[0] * i;
        for(index j = 0; j != shape[1]; ++j)
        {
          y = firstPosition[1] + gridSpacing[1] * j;
          for(index k = 0; k != shape[2]; ++k)
          {
            z = firstPosition[2] + gridSpacing[2] * k;
            (*grid)[i][j][k] = Vector(-s*z + dk_XZ * x, 
                                      dk_Y * y, 
                                      s*x + dk_XZ * z);
          }
        }
      }  
**/
      dynamics = new CartesianGrid3D(*grid, gridSpacing, firstPosition);
      double stepSize = 0.01;
      integrator = new RungeKutta4<CartesianGrid3D>(*dynamics, stepSize);
    }

  virtual ~CartesianGrid3DIntegrator()
    {
      delete dynamics;
      delete integrator;
      delete grid;
    }

  inline Vector step(const Point& p)
    {
      return integrator->step(p);
    }

  virtual unsigned int getModelVersion()
    {
      return dynamics->getModelVersion();
    }

  virtual CaveDialog* createParameterDialog(GLMotif::PopupMenu *parent)
    {
      return new CartesianGrid3DParameterDialog(parent, this);
    }

  virtual Vrui::Point center() const
    {
      return dynamics->center();
    }

  virtual bool autoShowParameterDialog()
    {
        return true;
    }
};

#endif
