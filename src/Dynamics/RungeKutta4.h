#ifndef RUNGEKUTTA4_H
#define RUNGEKUTTA4_H

#include "Integrator.h"

class RungeKutta4 : public Integrator<double>
{
private:

    /* Elements: */

    typedef void (RungeKutta4::*StepFunction)(Vector const& v, Vector & out);
    StepFunction stepFunction;

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

        addRealParameter( RealParameter("stepSize", stepSize, .0001, .2, .01, .0001) );

        int dimension = model.getDimension();

        switch (dimension)
        {
        case 0:
            throw IntegratorException();
            break;
        case 1:
            stepFunction = &RungeKutta4::step_1d;
            break;
        case 2:
            stepFunction = &RungeKutta4::step_2d;
            break;
        case 3:
            stepFunction = &RungeKutta4::step_3d;
            break;
        case 4:
            stepFunction = &RungeKutta4::step_4d;
            break;
        case 5:
            stepFunction = &RungeKutta4::step_5d;
            break;
        default:
            stepFunction = &RungeKutta4::step_nd;
            break;
        }
    }

    virtual ~RungeKutta4()
    {
    }

    /* Methods: */
    inline
    void step(Vector const& v, Vector &out)
    {
        // call pointer to member function
        (this->*stepFunction)(v, out);
    }

    // Computes one Runge-Kutta integration step vector
    void step_nd(Vector const& v, Vector &out)
    {
        Scalar stepSize = realParamValues[0];

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

    #include "RungeKutta4Step.inc.h"
};

#endif
