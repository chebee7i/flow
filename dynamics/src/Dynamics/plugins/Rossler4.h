#ifndef ROSSLER4_H
#define ROSSLER4_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

class Rossler4 : public DynamicalModel<double>
{
public:
    Rossler4(Scalar a=.25, Scalar b=3, Scalar c=.5, Scalar d=.05)
    : DynamicalModel<double>()
    {
        name = "Rossler4";

        // double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", -100, 40) );
        addCoordinate( Coordinate("y", -80, 60) );
        addCoordinate( Coordinate("z", -100, 40) );
        addCoordinate( Coordinate("w", -40, 100) );        

        addRealParameter( RealParameter("a", a, 0,    2.0,  0.25, 0.25) );
        addRealParameter( RealParameter("b", b, 0,    5.0,  3.00, 0.10) );
        addRealParameter( RealParameter("c", c, 0.1,  1.0,  0.50, 0.10) );
        addRealParameter( RealParameter("d", d, 0.01, 0.1,  0.05, 0.01) );        
    }

    virtual ~Rossler4() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = -p[1] - p[0];
        out[1] = p[0] + realParamValues[0] * p[1] + p[3];
        out[2] = realParamValues[1] + p[0] * p[2];
        out[3] = -realParamValues[2] * p[2] + realParamValues[3] * p[3];
    }
};

#endif
