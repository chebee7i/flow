#ifndef OWL_H
#define OWL_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

class Owl : public DynamicalModel<double>
{
public:
    Owl(Scalar a=10, Scalar b=10, Scalar c=13)
    : DynamicalModel<double>()
    {
        name = "Owl";

        double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", .5, -15, 15) );
        addCoordinate( Coordinate("y", .5, -15, 15) );
        addCoordinate( Coordinate("z", .5, 0, 20) );
        addCoordinate( Coordinate("t", 0, 0, inf) );        

        addRealParameter( RealParameter("a", a, -20, 20,  10, .01) );
        addRealParameter( RealParameter("b", b, -20, 20,  10, .01) );
        addRealParameter( RealParameter("c", c, -20, 20,  13, .01) );
        
        centerPoint.setDimension(4);
        centerPoint[0] = 0;
        centerPoint[1] = 0;
        centerPoint[2] = 0;
        centerPoint[3] = 0;
    }

    virtual ~Owl() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = -realParamValues[0] * (p[0] + p[1]);
        out[1] = -p[1] - realParamValues[1] * p[0] * p[2];
        out[2] = 10 * p[0] * p[1] + realParamValues[2];
        out[3] = 1;
    }
};

#endif
