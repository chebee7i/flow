#ifndef RUNGEKUTTA4_H
#define RUNGEKUTTA4_H

#include "Integrator.h"

class RungeKutta4 : public Integrator<double>
{
private:

    /* Elements: */

    // Vectors for intermediate calculations
    Vector v0;
    Vector v1;
    Vector v2;
    Vector vTemp;

public:

    /* Constructors and destructors: */

    RungeKutta4(const Model& model, Scalar stepSize=.01)
    : Integrator<double>(model),
      v0(model.getDimension()),
      v1(model.getDimension()),
      v2(model.getDimension()),
      vTemp(model.getDimension())
    {
        name = "rk4";

        addParameter( Parameter("stepSize", stepSize, .001, .1, .01, .001) );
    }

    virtual ~RungeKutta4()
    {
    }

    /* Methods: */

    // Computes one Runge-Kutta integration step vector
    void step(Vector const& v, Vector &out)
    {
        Scalar stepSize = paramValues[0];

        /* Calculate first half-step vector: */
        model(v, v0);
        v0 *= stepSize * Scalar(0.5);

        /* Calculate second half-step vector: */
        vTemp = v;
        vTemp += v0;
        model(vTemp, v1);
        v1 *= stepSize * Scalar(0.5);

        /* Calculate third half-step vector: */
        vTemp = v;
        vTemp += v1;
        model(vTemp, v2);
        v2 *= stepSize;

        /* Calculate fourth half-step vector: */
        vTemp = v;
        vTemp += v2;
        model(vTemp, out);
        out *= stepSize;

        /* Calculate step vector: */
        v1 *= Scalar(2);
        v2 += v1;
        v2 += v0;
        v2 *= Scalar(2);
        out += v2;
        out /= Scalar(6);
    }

};

#endif
