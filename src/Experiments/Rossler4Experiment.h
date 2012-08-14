#ifndef DTS_ROSSLER4EXPERIMENT
#define DTS_ROSSLER4EXPERIMENT

#include "Experiment.h"
#include "Models/Rossler4.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class Rossler4Experiment : public Experiment<double>
{
public:
    Rossler4Experiment() : Experiment<double>()
    {
        model = new Rossler4();

        addIntegrator( new RungeKutta4(*model, .02) );
        setIntegrator("rk4");
        
        ProjectionTransformer<double> *t;
        t = new ProjectionTransformer<double>(*model);
        
        t->setIntParamValue("xDisplay", 0);
        t->setIntParamValue("yDisplay", 1);
        t->setIntParamValue("zDisplay", 2);

        addTransformer( t );
        setTransformer("projection");      
    }
};

#endif
