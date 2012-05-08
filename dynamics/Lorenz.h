#ifndef LORENZ_H
#define LORENZ_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

#include <RungeKutta4.h>
#include <Experiment.h>

class Lorenz : public DynamicalModel<double>
{
public:
    Lorenz(Scalar sigma=10, Scalar rho=28, Scalar beta=8/3.0)
    : DynamicalModel<double>()
    {
        name = "Lorenz";

        // double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", -25, 25) );
        addCoordinate( Coordinate("y", -10, 10) );
        addCoordinate( Coordinate("z", -1, 40) );

        addParameter( Parameter("sigma", sigma, 0, 20,  10,    0.1) );
        addParameter( Parameter("rho",   rho,   0, 100, 28,    1.0) );
        addParameter( Parameter("beta",  beta,  0, 10,  8/3.0, 0.1) );
    }

    virtual ~Lorenz() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = paramValues[0] * (p[1] - p[0]);
        out[1] = paramValues[1] * p[0] - p[1] - p[0] * p[2];
        out[2] = p[0] * p[1] - paramValues[2] * p[2];
    }
};

class LorenzExperiment : public Experiment<double>
{
public:
    LorenzExperiment()
    : Experiment<double>()
    {
        model = new Lorenz();

        addIntegrator( new RungeKutta4(*model, .01) );

        setIntegrator("rk4");
    }
};

#endif
