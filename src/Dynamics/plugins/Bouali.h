#ifndef BOUALI_H
#define BOUALI_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

// http://arxiv.org/abs/1204.0045
class Bouali : public DynamicalModel<double>
{
public:
    Bouali(Scalar alpha=0.3, Scalar s=1)
    : DynamicalModel<double>()
    {
        name = "Bouali";

        double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", -3, -5, 5) );
        addCoordinate( Coordinate("y", .6, 0, 20) );
        addCoordinate( Coordinate("z", 1.2, -5, 5) );
        addCoordinate( Coordinate("t", 0, 0, inf) );

        addRealParameter( RealParameter("alpha", alpha, 0, 10,  .3,    0.01) );
        addRealParameter( RealParameter("s",   s,   0, 8, 1, 0.01) );

        centerPoint.setDimension(4);
        centerPoint[0] = -1;
        centerPoint[1] = 0;
        centerPoint[2] = -5;
        centerPoint[3] = 0;

    }

    virtual ~Bouali() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = p[0] * (4 - p[1]) + realParamValues[0] * p[2];
        out[1] = -p[1] * (1 - p[0] * p[0]);
        out[2] = -p[0] * (1.5 - realParamValues[1] * p[2]) - 0.05 * p[2];
        out[3] = 1;
    }
};

#endif
