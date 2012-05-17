#ifndef ROSSLER4_H
#define ROSSLER4_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

class Rossler4 : public DynamicalModel<double>
{
public:
    Rossler4(Scalar a=.25,  Scalar b=-.5, Scalar c=2.2, Scalar d=.05)
    : DynamicalModel<double>()
    {
        name = "Hyperchaos";

        double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", -20, -130, 30) );
        addCoordinate( Coordinate("y", 0, -80, 10) );
        addCoordinate( Coordinate("z", 0, 0, 30) );
        addCoordinate( Coordinate("w", 15, 0, 70) );        
        addCoordinate( Coordinate("t", 0, 0, inf) );                

        addRealParameter( RealParameter("a", a, 0,    2.0,  0.25, 0.01) );
        addRealParameter( RealParameter("b", b, -2,   2.0, -0.50, 0.01) );
        addRealParameter( RealParameter("c", c, 0,    5.0,  2.20, 0.01) );
        addRealParameter( RealParameter("d", d, -0.5, 0.5,  0.05, 0.01) );      

        centerPoint.setDimension(5);
        centerPoint[0] = -50;
        centerPoint[1] = -35;
        centerPoint[2] = 40;
        centerPoint[3] = 35;
        centerPoint[4] = 0;
          
    }

    virtual ~Rossler4() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = -p[1] - p[2];
        out[1] = p[0] + realParamValues[0] * p[1] + p[3];
        out[2] = realParamValues[2] + p[0] * p[2];
        out[3] = realParamValues[1] * p[2] + realParamValues[3] * p[3];
        out[4] = 1;
    }
};

#endif
